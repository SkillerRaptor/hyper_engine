/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    bindings::{BindingsOffset, TexturedBindings},
    mesh::{Mesh, Vertex},
    render_context::Frame,
    render_object::RenderObject,
    resource_handle::ResourceHandle,
    vulkan::{
        command::{command_buffer::CommandBuffer, command_pool::CommandPool},
        core::{device::Device, instance::Instance, surface::Surface, swapchain::Swapchain},
        descriptors::descriptor_manager::DescriptorManager,
        memory::allocator::{Allocator, MemoryLocation},
        pipeline::{
            graphics_pipelines::{
                ColorBlendAttachmentStateCreateInfo, ColorBlendStateCreateInfo,
                DepthStencilStateCreateInfo, GraphicsPipeline, GraphicsPipelineCreateInfo,
                InputAssemblyCreateInfo, RasterizationStateCreateInfo,
            },
            pipeline_layout::PipelineLayout,
        },
        resource::{
            buffer::Buffer, shader::Shader, texture::Texture, upload_manager::UploadManager,
        },
        sync::{binary_semaphore::BinarySemaphore, timeline_semaphore::TimelineSemaphore},
    },
};

use hyper_math::{
    matrix::Mat4x4f,
    vector::{Vec3f, Vec4f},
};
use hyper_platform::window::Window;

use ash::vk;
use std::{cell::RefCell, collections::HashMap, mem, rc::Rc};

pub(crate) struct Renderer {
    frame_buffer: Buffer,
    upload_manager: Rc<RefCell<UploadManager>>,

    // TODO: Make a material system
    _textures: HashMap<String, Texture>,
    materials: HashMap<String, GraphicsPipeline>,
    meshes: HashMap<String, Mesh>,
    // NOTE: Temporary
    renderables: Vec<RenderObject>,

    _projection_view_buffer: Buffer,

    current_frame_id: u64,
    swapchain_image_index: u32,

    submit_semaphore: TimelineSemaphore,

    render_semaphores: Vec<BinarySemaphore>,
    present_semaphores: Vec<BinarySemaphore>,

    command_buffers: Vec<CommandBuffer>,
    _command_pool: CommandPool,

    descriptor_manager: Rc<RefCell<DescriptorManager>>,
    device: Rc<Device>,
}

impl Renderer {
    const FRAMES_IN_FLIGHT: u32 = 2;

    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        swapchain: &Swapchain,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_manager: Rc<RefCell<UploadManager>>,
        pipeline_layout: &PipelineLayout,
    ) -> Self {
        let command_pool = CommandPool::new(instance, surface, device.clone());

        let mut command_buffers = Vec::new();
        let mut present_semaphores = Vec::new();
        let mut render_semaphores = Vec::new();
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let command_buffer = CommandBuffer::new(device.clone(), &command_pool);
            let present_semaphore = BinarySemaphore::new(device.clone());
            let render_semaphore = BinarySemaphore::new(device.clone());

            command_buffers.push(command_buffer);
            present_semaphores.push(present_semaphore);
            render_semaphores.push(render_semaphore);
        }

        let submit_semaphore = TimelineSemaphore::new(device.clone());

        ////////////////////////////////////////////////////////////////////////

        let mut materials = HashMap::new();

        let vertex_shader = Shader::new(device.clone(), "./assets/shaders/compiled/default_vs.spv");
        let fragment_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_fs.spv");
        let default_pipeline = GraphicsPipeline::new(
            device.clone(),
            GraphicsPipelineCreateInfo {
                layout: pipeline_layout,

                vertex_shader: Some(vertex_shader),
                fragment_shader: Some(fragment_shader),

                color_image_format: swapchain.format(),
                depth_image_format: Some(swapchain.depth_image_format()),

                input_assembly: InputAssemblyCreateInfo {
                    toplogy: vk::PrimitiveTopology::TRIANGLE_LIST,
                    ..Default::default()
                },
                rasterization_state: RasterizationStateCreateInfo {
                    polygon_mode: vk::PolygonMode::FILL,
                    cull_mode: vk::CullModeFlags::BACK,
                    front_face: vk::FrontFace::COUNTER_CLOCKWISE,

                    depth_bias_enable: false,
                    ..Default::default()
                },
                depth_stencil_state: DepthStencilStateCreateInfo {
                    depth_test_enable: true,
                    depth_write_enable: true,
                    depth_compare_op: vk::CompareOp::LESS_OR_EQUAL,

                    depth_bounds_test_enable: false,
                    ..Default::default()
                },
                color_blend_attachment_state: ColorBlendAttachmentStateCreateInfo {
                    blend_enable: false,
                    ..Default::default()
                },
                color_blend_state: ColorBlendStateCreateInfo {
                    logic_op_enable: false,
                    ..Default::default()
                },
            },
        );

        materials.insert("default".to_string(), default_pipeline);

        let vertex_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/textured_vs.spv");
        let fragment_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/textured_fs.spv");
        let textured_pipeline = GraphicsPipeline::new(
            device.clone(),
            GraphicsPipelineCreateInfo {
                layout: pipeline_layout,

                vertex_shader: Some(vertex_shader),
                fragment_shader: Some(fragment_shader),

                color_image_format: swapchain.format(),
                depth_image_format: Some(swapchain.depth_image_format()),

                input_assembly: InputAssemblyCreateInfo {
                    toplogy: vk::PrimitiveTopology::TRIANGLE_LIST,
                    ..Default::default()
                },
                rasterization_state: RasterizationStateCreateInfo {
                    polygon_mode: vk::PolygonMode::FILL,
                    cull_mode: vk::CullModeFlags::BACK,
                    front_face: vk::FrontFace::COUNTER_CLOCKWISE,

                    depth_bias_enable: false,
                    ..Default::default()
                },
                depth_stencil_state: DepthStencilStateCreateInfo {
                    depth_test_enable: true,
                    depth_write_enable: true,
                    depth_compare_op: vk::CompareOp::LESS_OR_EQUAL,

                    depth_bounds_test_enable: false,
                    ..Default::default()
                },
                color_blend_attachment_state: ColorBlendAttachmentStateCreateInfo {
                    blend_enable: false,
                    color_write_mask: vk::ColorComponentFlags::RGBA,
                    ..Default::default()
                },
                color_blend_state: ColorBlendStateCreateInfo {
                    logic_op_enable: false,
                    ..Default::default()
                },
            },
        );

        materials.insert("textured".to_string(), textured_pipeline);

        ////////////////////////////////////////////////////////////////////////

        let mut meshes = HashMap::new();

        let triangle_vertices = vec![
            Vertex {
                position: Vec4f::new(1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(1.0, 0.0, 0.0, 1.0),
                uv: Vec4f::default(),
            },
            Vertex {
                position: Vec4f::new(0.0, -1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 0.0, 1.0, 1.0),
                uv: Vec4f::default(),
            },
            Vertex {
                position: Vec4f::new(-1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 1.0, 0.0, 1.0),
                uv: Vec4f::default(),
            },
        ];

        let triangle_mesh = Mesh::new(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            upload_manager.clone(),
            triangle_vertices,
            None,
        );

        meshes.insert("triangle".to_string(), triangle_mesh);

        let monkey_mesh = Mesh::load(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            upload_manager.clone(),
            "./assets/models/monkey_smooth.obj",
        );

        meshes.insert("monkey".to_string(), monkey_mesh);

        let lost_empire_mesh = Mesh::load(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            upload_manager.clone(),
            "./assets/models/lost_empire.obj",
        );

        meshes.insert("lost_empire".to_string(), lost_empire_mesh);

        ////////////////////////////////////////////////////////////////////////

        let mut textures = HashMap::new();

        let lost_empire = Texture::new(
            device.clone(),
            allocator.clone(),
            upload_manager.clone(),
            "./assets/textures/lost_empire-RGBA.png",
        );

        textures.insert("lost_empire".to_string(), lost_empire);

        ////////////////////////////////////////////////////////////////////////

        // TODO: Abstract into camera struct and add dynamic update
        let projection_view_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        );

        let camera_position = Vec3f::new(7.0, -15.0, -6.0);

        let mut view_matrix = Mat4x4f::identity();
        view_matrix.append_translation_mut(&camera_position);

        let projection_matrix =
            Mat4x4f::new_perspective(f32::to_radians(90.0), 1280.0 / 720.0, 0.1, 200.0);

        let projection_view = projection_matrix * view_matrix;
        upload_manager
            .borrow_mut()
            .upload_buffer(&[projection_view], &projection_view_buffer);

        descriptor_manager
            .borrow_mut()
            .update_camera(&projection_view_buffer);

        ////////////////////////////////////////////////////////////////////////

        let sampler_create_info = vk::SamplerCreateInfo::builder()
            .mag_filter(vk::Filter::NEAREST)
            .min_filter(vk::Filter::NEAREST)
            .address_mode_u(vk::SamplerAddressMode::REPEAT)
            .address_mode_v(vk::SamplerAddressMode::REPEAT)
            .address_mode_w(vk::SamplerAddressMode::REPEAT);

        let sampler = unsafe {
            device
                .handle()
                .create_sampler(&sampler_create_info, None)
                .expect("failed to create sampler")
        };

        let combined_image_sampler_handle = descriptor_manager
            .borrow_mut()
            .allocate_combined_image_sampler_handle(
                sampler,
                textures["lost_empire"].view(),
                vk::ImageLayout::SHADER_READ_ONLY_OPTIMAL,
            );

        ////////////////////////////////////////////////////////////////////////

        let mut renderables = Vec::new();

        let lost_empire = RenderObject::new::<TexturedBindings>(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            upload_manager.clone(),
            "lost_empire",
            "textured",
            [Mat4x4f::identity()].to_vec(),
            meshes["lost_empire"].vertex_buffer_handle(),
            &[combined_image_sampler_handle],
        );
        renderables.push(lost_empire);

        let frame_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Frame>(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        );

        Self {
            frame_buffer,
            upload_manager: upload_manager.clone(),

            _textures: textures,
            materials,
            meshes,
            renderables,

            _projection_view_buffer: projection_view_buffer,

            current_frame_id: 0,
            swapchain_image_index: 0,

            submit_semaphore,

            render_semaphores,
            present_semaphores,

            command_buffers,
            _command_pool: command_pool,

            descriptor_manager,
            device,
        }
    }

    pub(crate) fn begin(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        frame_id: u64,
        swapchain: &mut Swapchain,
    ) {
        self.current_frame_id = frame_id;
        self.submit_semaphore.wait_for(frame_id - 1);

        let side = self.current_frame_id % 2;

        let Some(index) = swapchain.acquire_next_image(
            window,
            instance,
            surface,
            &self.present_semaphores[side as usize]) else {
            return;
        };

        self.swapchain_image_index = index;

        self.command_buffers[side as usize].reset();
        self.command_buffers[side as usize].begin(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);

        let subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
            .src_stage_mask(vk::PipelineStageFlags2::TOP_OF_PIPE)
            .dst_stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .dst_access_mask(vk::AccessFlags2::COLOR_ATTACHMENT_WRITE)
            .old_layout(vk::ImageLayout::UNDEFINED)
            .new_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.swapchain_image_index as usize])
            .subresource_range(*subresource_range);

        let image_memory_barriers = [*image_memory_barrier];
        let dependency_info = vk::DependencyInfo::builder()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        self.command_buffers[side as usize].pipeline_barrier2(*dependency_info);
    }

    pub(crate) fn end(&self, swapchain: &Swapchain) {
        let side = self.current_frame_id % 2;

        let subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
            .src_stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .src_access_mask(vk::AccessFlags2::COLOR_ATTACHMENT_WRITE)
            .dst_stage_mask(vk::PipelineStageFlags2::BOTTOM_OF_PIPE)
            .old_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .new_layout(vk::ImageLayout::PRESENT_SRC_KHR)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.swapchain_image_index as usize])
            .subresource_range(*subresource_range);

        let image_memory_barriers = [*image_memory_barrier];

        let dependency_info = vk::DependencyInfo::builder()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        self.command_buffers[side as usize].pipeline_barrier2(*dependency_info);

        self.command_buffers[side as usize].end();
    }

    pub(crate) fn begin_rendering(&mut self, swapchain: &mut Swapchain) {
        let side = self.current_frame_id % 2;

        let render_area_extent = swapchain.extent();
        let render_area_offset = vk::Offset2D::builder().x(0).y(0);

        let render_area = vk::Rect2D::builder()
            .extent(render_area_extent)
            .offset(*render_area_offset);

        let color_clear = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.12941, 0.06275, 0.13725, 1.0],
            },
        };

        let depth_clear = vk::ClearValue {
            depth_stencil: vk::ClearDepthStencilValue {
                depth: 1.0,
                stencil: 0,
            },
        };

        let color_attachment_info = vk::RenderingAttachmentInfo::builder()
            .image_view(swapchain.image_views()[self.swapchain_image_index as usize])
            .image_layout(vk::ImageLayout::ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(color_clear);

        let color_attachments = [*color_attachment_info];

        let depth_attachment_info = vk::RenderingAttachmentInfo::builder()
            .image_view(swapchain.depth_image_view())
            .image_layout(vk::ImageLayout::DEPTH_ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(depth_clear);

        let rendering_info = vk::RenderingInfo::builder()
            .render_area(*render_area)
            .layer_count(1)
            .color_attachments(&color_attachments)
            .depth_attachment(&depth_attachment_info);

        self.command_buffers[side as usize].begin_rendering(*rendering_info);

        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(swapchain.extent().height as f32)
            .width(swapchain.extent().width as f32)
            .height(-(swapchain.extent().height as f32))
            .min_depth(0.0)
            .max_depth(1.0);

        self.command_buffers[side as usize].set_viewport(0, &[*viewport]);

        let offset = vk::Offset2D::builder().x(0).y(0);

        let scissor = vk::Rect2D::builder()
            .offset(*offset)
            .extent(swapchain.extent());

        self.command_buffers[side as usize].set_scissor(0, &[*scissor]);
    }

    pub(crate) fn begin_rendering_gui(&mut self, swapchain: &mut Swapchain) {
        let side = self.current_frame_id % 2;

        let render_area_extent = swapchain.extent();
        let render_area_offset = vk::Offset2D::builder().x(0).y(0);

        let render_area = vk::Rect2D::builder()
            .extent(render_area_extent)
            .offset(*render_area_offset);

        let color_attachment_info = vk::RenderingAttachmentInfo::builder()
            .image_view(swapchain.image_views()[self.swapchain_image_index as usize])
            .image_layout(vk::ImageLayout::ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::LOAD)
            .store_op(vk::AttachmentStoreOp::STORE);

        let color_attachments = [*color_attachment_info];

        let rendering_info = vk::RenderingInfo::builder()
            .render_area(*render_area)
            .layer_count(1)
            .color_attachments(&color_attachments);

        self.command_buffers[side as usize].begin_rendering(*rendering_info);

        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(swapchain.extent().height as f32)
            .width(swapchain.extent().width as f32)
            .height(-(swapchain.extent().height as f32))
            .min_depth(0.0)
            .max_depth(1.0);

        self.command_buffers[side as usize].set_viewport(0, &[*viewport]);

        let offset = vk::Offset2D::builder().x(0).y(0);

        let scissor = vk::Rect2D::builder()
            .offset(*offset)
            .extent(swapchain.extent());

        self.command_buffers[side as usize].set_scissor(0, &[*scissor]);
    }

    pub(crate) fn end_rendering(&self) {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize].end_rendering();
    }

    pub(crate) fn submit(
        &self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) {
        let side = self.current_frame_id % 2;

        self.device.submit_render_queue(
            &self.command_buffers[side as usize],
            &self.present_semaphores[side as usize],
            &self.render_semaphores[side as usize],
            &self.submit_semaphore,
            self.current_frame_id,
        );

        swapchain.present_queue(
            window,
            instance,
            surface,
            self.device.present_queue(),
            &self.render_semaphores[side as usize],
            self.swapchain_image_index,
        );
    }

    pub(crate) fn draw_objects(&self, pipeline_layout: &PipelineLayout) {
        // TODO: Draw scene and not a triangle
        let side = self.current_frame_id % 2;

        // TODO: Update/Set camera dynamically

        let mut last_material = String::new();
        for renderable in &self.renderables {
            let current_material = renderable.material();
            if current_material != last_material {
                let material = &self.materials[current_material];

                self.command_buffers[side as usize]
                    .bind_pipeline(vk::PipelineBindPoint::GRAPHICS, material);

                let descriptor_sets = self
                    .descriptor_manager
                    .borrow()
                    .descriptor_sets()
                    .iter()
                    .map(|descriptor_set| descriptor_set.handle())
                    .collect::<Vec<_>>();
                self.command_buffers[side as usize].bind_descriptor_sets(
                    vk::PipelineBindPoint::GRAPHICS,
                    pipeline_layout,
                    0,
                    &descriptor_sets,
                    &[],
                );

                last_material = current_material.to_string();
            }

            let bindings_offset = BindingsOffset::new(renderable.bindings_handle());
            self.command_buffers[side as usize].push_constants(
                pipeline_layout,
                vk::ShaderStageFlags::ALL,
                0,
                &bindings_offset,
            );

            let mesh = &self.meshes[renderable.mesh()];

            if mesh.indices_count() == 0 {
                self.command_buffers[side as usize].draw(
                    mesh.vertices().len() as u32,
                    renderable.transforms().len() as u32,
                    0,
                    0,
                );
            } else {
                self.command_buffers[side as usize]
                    .bind_index_buffer(mesh.index_buffer().as_ref().unwrap());
                self.command_buffers[side as usize].draw_indexed(
                    mesh.indices_count() as u32,
                    renderable.transforms().len() as u32,
                    0,
                    0,
                    0,
                )
            }
        }
    }

    pub(crate) fn resize(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) {
        swapchain.recreate(window, instance, surface);
    }

    pub(crate) fn bind_pipeline(
        &self,
        pipeline: &GraphicsPipeline,
        pipeline_layout: &PipelineLayout,
    ) {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize]
            .bind_pipeline(vk::PipelineBindPoint::GRAPHICS, pipeline);

        let descriptor_sets = self
            .descriptor_manager
            .borrow()
            .descriptor_sets()
            .iter()
            .map(|descriptor_set| descriptor_set.handle())
            .collect::<Vec<_>>();
        self.command_buffers[side as usize].bind_descriptor_sets(
            vk::PipelineBindPoint::GRAPHICS,
            pipeline_layout,
            0,
            &descriptor_sets,
            &[],
        );
    }

    pub(crate) fn set_scissor(&self, offset: vk::Offset2D, extent: vk::Extent2D) {
        let side = self.current_frame_id % 2;

        let scissor = vk::Rect2D::builder().offset(offset).extent(extent);

        self.command_buffers[side as usize].set_scissor(0, &[*scissor]);
    }

    pub(crate) fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize].draw_indexed(
            index_count,
            instance_count,
            first_index,
            vertex_offset,
            first_instance,
        );
    }

    pub(crate) fn push_constants(
        &self,
        pipeline_layout: &PipelineLayout,
        bindings_handle: ResourceHandle,
    ) {
        let side = self.current_frame_id % 2;

        let bindings_offset = BindingsOffset::new(bindings_handle);
        self.command_buffers[side as usize].push_constants(
            pipeline_layout,
            vk::ShaderStageFlags::ALL,
            0,
            &bindings_offset,
        );
    }

    pub(crate) fn bind_index_buffer(&self, index_buffer: &Buffer) {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize].bind_index_buffer(index_buffer);
    }

    pub(crate) fn update_frame(&self, frame: Frame) {
        self.upload_manager
            .borrow_mut()
            .upload_buffer(&[frame], &self.frame_buffer);

        self.descriptor_manager
            .borrow_mut()
            .update_frame(&self.frame_buffer);
    }
}
