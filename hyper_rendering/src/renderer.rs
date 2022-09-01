/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use std::collections::HashMap;

use crate::{
    allocator::Allocator,
    commands::{command_buffer::CommandBuffer, command_pool::CommandPool},
    devices::{device::Device, surface::Surface},
    mesh::Mesh,
    pipeline::{
        pipeline::{MeshPushConstants, Pipeline},
        swapchain::Swapchain,
    },
    render_object::{Material, RenderObject},
    sync::{fence::Fence, semaphore::Semaphore},
    vertex::Vertex,
};

use hyper_platform::window::Window;

use ash::vk::{
    self, AccessFlags, AttachmentLoadOp, AttachmentStoreOp, ClearColorValue,
    ClearDepthStencilValue, ClearValue, CommandBufferLevel, CommandBufferResetFlags,
    CommandBufferUsageFlags, DependencyFlags, ImageAspectFlags, ImageLayout, ImageMemoryBarrier,
    ImageSubresourceRange, ImageView, Offset2D, PipelineBindPoint, PipelineStageFlags,
    PresentInfoKHR, Rect2D, RenderingAttachmentInfo, RenderingInfo, ResolveModeFlags,
    ShaderStageFlags, SubmitInfo, Viewport,
};
use glm::vec3;
use log::info;
use nalgebra_glm as glm;
use tracing::instrument;

pub(crate) struct Renderer {
    current_image_index: usize,

    command_pool: CommandPool,
    command_buffer: CommandBuffer,

    render_fence: Fence,
    present_semaphore: Semaphore,
    render_semaphore: Semaphore,

    materials: HashMap<String, Material>,
    meshes: HashMap<String, Mesh>,
    renderables: Vec<RenderObject>,
}

impl Renderer {
    #[instrument(skip_all)]
    pub fn new(device: &Device, pipeline: &Pipeline, allocator: &mut Allocator) -> Self {
        let command_pool = CommandPool::new(device);
        let command_buffer = CommandBuffer::new(device, &command_pool, CommandBufferLevel::PRIMARY);

        let render_fence = Fence::new(device);
        let present_semaphore = Semaphore::new(device);
        let render_semaphore = Semaphore::new(device);

        let material = Material {
            pipeline: *pipeline.pipeline(),
            pipeline_layout: *pipeline.pipeline_layout(),
        };

        let mut materials = HashMap::new();
        materials.insert(String::from("default"), material);

        // NOTE: This is temporary mesh loading
        let triangle_vertices = vec![
            Vertex::new(
                vec3(1.0, 1.0, 0.5),
                vec3(1.0, 0.0, 0.0),
                vec3(0.0, 0.0, 0.0),
            ),
            Vertex::new(
                vec3(-1.0, 1.0, 0.5),
                vec3(0.0, 1.0, 0.0),
                vec3(0.0, 0.0, 0.0),
            ),
            Vertex::new(
                vec3(0.0, -1.0, 0.5),
                vec3(0.0, 0.0, 1.0),
                vec3(0.0, 0.0, 0.0),
            ),
        ];

        let triangle_mesh = Mesh::new(device, allocator, &triangle_vertices);
        let monkey_mesh = Mesh::load(device, allocator, "assets/models/monkey_smooth.obj");

        let mut meshes = HashMap::new();
        meshes.insert(String::from("triangle"), triangle_mesh);
        meshes.insert(String::from("monkey"), monkey_mesh);

        let mut renderables = Vec::new();

        let monkey_render_object = RenderObject {
            mesh: String::from("monkey"),
            material: String::from("default"),
            transform: glm::mat4(
                1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
            ),
        };

        renderables.push(monkey_render_object);

        for x in -20..20 {
            for z in -20..20 {
                let translation = glm::translate(
                    &glm::mat4(
                        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                        1.0,
                    ),
                    &vec3(x as f32, 0.0, z as f32),
                );

                let scale = glm::scale(
                    &glm::mat4(
                        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                        1.0,
                    ),
                    &vec3(0.2, 0.2, 0.2),
                );

                let triangle_render_object = RenderObject {
                    mesh: String::from("triangle"),
                    material: String::from("default"),
                    transform: translation * scale,
                };

                renderables.push(triangle_render_object);
            }
        }

        info!("Created renderer");
        Self {
            current_image_index: 0,

            command_pool,
            command_buffer,

            render_fence,
            present_semaphore,
            render_semaphore,

            materials,
            meshes,
            renderables,
        }
    }

    #[instrument(skip_all)]
    pub fn cleanup(&mut self, device: &Device, allocator: &mut Allocator) {
        for mesh in &mut self.meshes.values_mut() {
            mesh.cleanup(device, allocator);
        }

        self.render_semaphore.cleanup(device);
        self.present_semaphore.cleanup(device);
        self.render_fence.cleanup(device);
        self.command_pool.cleanup(device);
    }

    #[instrument(skip_all)]
    pub fn begin_frame(
        &mut self,
        window: &Window,
        surface: &Surface,
        device: &Device,
        allocator: &mut Allocator,
        swapchain: &mut Swapchain,
        _pipeline: &Pipeline,
    ) {
        self.render_fence.wait(device);
        self.render_fence.reset(device);

        unsafe {
            // TODO: Move this to swapchain class
            match swapchain.swapchain_loader().acquire_next_image(
                *swapchain.swapchain(),
                u64::MAX,
                *self.present_semaphore.semaphore(),
                vk::Fence::null(),
            ) {
                Ok((image_index, _)) => self.current_image_index = image_index as usize,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    swapchain.recreate(window, surface, device, allocator);
                    return;
                }
                Err(_error) => panic!(),
            }
        }

        self.command_buffer
            .reset(device, CommandBufferResetFlags::empty());

        self.command_buffer.begin(
            device,
            CommandBufferUsageFlags::ONE_TIME_SUBMIT,
            &vk::CommandBufferInheritanceInfo::default(),
        );

        let image_subresource_range = ImageSubresourceRange::builder()
            .aspect_mask(ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = ImageMemoryBarrier::builder()
            .src_access_mask(AccessFlags::empty())
            .dst_access_mask(AccessFlags::COLOR_ATTACHMENT_WRITE)
            .old_layout(ImageLayout::UNDEFINED)
            .new_layout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.current_image_index as usize])
            .subresource_range(*image_subresource_range);

        self.command_buffer.pipeline_barrier(
            device,
            PipelineStageFlags::TOP_OF_PIPE,
            PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

        let flash = f32::abs(f32::sin(window.time() as f32 * 2.0));
        let color_clear_value = ClearValue {
            color: ClearColorValue {
                float32: [0.0, 0.0, flash, 1.0],
            },
        };

        let offset = Offset2D::builder();
        let rendering_area = Rect2D::builder()
            .offset(*offset)
            .extent(*swapchain.extent());

        let color_attachment_info = RenderingAttachmentInfo::builder()
            .image_view(swapchain.image_views()[self.current_image_index as usize])
            .image_layout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .resolve_mode(ResolveModeFlags::NONE)
            .resolve_image_view(ImageView::null())
            .resolve_image_layout(ImageLayout::UNDEFINED)
            .load_op(AttachmentLoadOp::CLEAR)
            .store_op(AttachmentStoreOp::STORE)
            .clear_value(color_clear_value);

        let depth_clear_value = ClearValue {
            depth_stencil: ClearDepthStencilValue::builder()
                .depth(1.0)
                .stencil(0)
                .build(),
        };

        let depth_attachment_info = RenderingAttachmentInfo::builder()
            .image_view(*swapchain.depth_image_view())
            .image_layout(ImageLayout::DEPTH_ATTACHMENT_OPTIMAL)
            .resolve_mode(ResolveModeFlags::NONE)
            .resolve_image_view(ImageView::null())
            .resolve_image_layout(ImageLayout::UNDEFINED)
            .load_op(AttachmentLoadOp::CLEAR)
            .store_op(AttachmentStoreOp::STORE)
            .clear_value(depth_clear_value);

        let stencil_attachment_info = RenderingAttachmentInfo::builder();

        let color_atachments = &[*color_attachment_info];
        let rendering_info = RenderingInfo::builder()
            .render_area(*rendering_area)
            .layer_count(1)
            .view_mask(0)
            .color_attachments(color_atachments)
            .depth_attachment(&depth_attachment_info)
            .stencil_attachment(&stencil_attachment_info);

        self.command_buffer.begin_rendering(device, &rendering_info);
    }

    #[instrument(skip_all)]
    pub fn end_frame(&self, device: &Device, swapchain: &Swapchain) {
        self.command_buffer.end_rendering(device);

        let image_subresource_range = ImageSubresourceRange::builder()
            .aspect_mask(ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = ImageMemoryBarrier::builder()
            .src_access_mask(AccessFlags::COLOR_ATTACHMENT_WRITE)
            .dst_access_mask(AccessFlags::empty())
            .old_layout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .new_layout(ImageLayout::PRESENT_SRC_KHR)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.current_image_index as usize])
            .subresource_range(*image_subresource_range);

        self.command_buffer.pipeline_barrier(
            device,
            PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            PipelineStageFlags::BOTTOM_OF_PIPE,
            DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

        self.command_buffer.end(device);
    }

    #[instrument(skip_all)]
    pub fn submit(
        &mut self,
        window: &Window,
        surface: &Surface,
        device: &Device,
        allocator: &mut Allocator,
        swapchain: &mut Swapchain,
    ) {
        let wait_semaphores = &[*self.present_semaphore.semaphore()];
        let wait_stages = &[PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[*self.command_buffer.command_buffer()];
        let signal_semaphores = &[*self.render_semaphore.semaphore()];
        let submit_info = SubmitInfo::builder()
            .wait_semaphores(wait_semaphores)
            .wait_dst_stage_mask(wait_stages)
            .command_buffers(command_buffers)
            .signal_semaphores(signal_semaphores);

        let submits = &[*submit_info];
        unsafe {
            // TODO: Create queue class and move this to queue class
            device
                .logical_device()
                .queue_submit(
                    *device.graphics_queue(),
                    submits,
                    *self.render_fence.fence(),
                )
                .expect("Failed to submit to queue");
        }

        let wait_semaphores = &[*self.render_semaphore.semaphore()];
        let swapchains = &[*swapchain.swapchain()];
        let image_indices = &[self.current_image_index as u32];

        let present_info = PresentInfoKHR::builder()
            .wait_semaphores(wait_semaphores)
            .image_indices(image_indices)
            .swapchains(swapchains);

        unsafe {
            // TODO: Move this to swapchain class
            let changed = match swapchain
                .swapchain_loader()
                .queue_present(*device.graphics_queue(), &present_info)
            {
                Ok(suboptimal) => suboptimal,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(_error) => panic!(),
            };

            if changed {
                swapchain.recreate(window, surface, device, allocator);
            }
        }
    }

    #[instrument(skip_all)]
    pub fn draw(
        &self,
        window: &Window,
        device: &Device,
        swapchain: &Swapchain,
        _pipeline: &Pipeline,
    ) {
        let camera_position = glm::vec3(0.0, 0.0, -2.0);

        let view_matrix = glm::translate(
            &glm::mat4(
                1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
            ),
            &camera_position,
        );

        let mut projection_matrix = glm::perspective(
            f32::to_radians(90.0),
            window.framebuffer_width() as f32 / window.framebuffer_height() as f32,
            0.1,
            200.0,
        );

        projection_matrix.m22 *= -1_f32;

        //
        for render_object in &self.renderables {
            let mesh = self.meshes.get(&render_object.mesh).unwrap();

            let material = self.materials.get(&render_object.material).unwrap();
            let pipeline = material.pipeline;
            let pipeline_layout = material.pipeline_layout;

            self.command_buffer
                .bind_pipeline(device, PipelineBindPoint::GRAPHICS, pipeline);

            let extent = swapchain.extent();
            let viewport = Viewport::builder()
                .x(0.0)
                .y(0.0)
                .width(extent.width as f32)
                .height(extent.height as f32)
                .min_depth(0.0)
                .max_depth(1.0);
            self.command_buffer.set_viewport(device, 0, &[*viewport]);

            let offset = Offset2D::builder();
            let scissor = Rect2D::builder().offset(*offset).extent(*extent);
            self.command_buffer.set_scissor(device, 0, &[*scissor]);

            let model_matrix = render_object.transform;

            let mesh_matrix = projection_matrix * view_matrix * model_matrix;

            let push_constants = MeshPushConstants {
                data: glm::vec4(0.0, 0.0, 0.0, 0.0),
                render_matrix: mesh_matrix,
            };

            self.command_buffer.push_constants(
                device,
                pipeline_layout,
                ShaderStageFlags::VERTEX,
                0,
                &push_constants,
            );

            let buffers = &[*mesh.vertex_buffer().internal_buffer()];
            let offsets = &[0];

            self.command_buffer
                .bind_vertex_buffers(device, 0, buffers, offsets);

            self.command_buffer
                .draw(device, mesh.vertices().len() as u32, 1, 0, 0);
        }
    }
}
