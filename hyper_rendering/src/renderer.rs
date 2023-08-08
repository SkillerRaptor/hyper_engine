/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, MemoryLocation},
    binary_semaphore::BinarySemaphore,
    buffer::Buffer,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationError, CreationResult, RuntimeError, RuntimeResult},
    instance::Instance,
    mesh::{Mesh, Vertex},
    pipeline::{BindingsOffset, Pipeline},
    render_object::RenderObject,
    resource_handle::ResourceHandle,
    shader::Shader,
    surface::Surface,
    swapchain::Swapchain,
    timeline_semaphore::TimelineSemaphore,
};

use hyper_math::{
    matrix::Mat4x4f,
    vector::{Vec3f, Vec4f},
};
use hyper_platform::window::Window;

use ash::vk;
use std::{cell::RefCell, collections::HashMap, fmt::Debug, mem, rc::Rc};

// NOTE: Temporary
#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct Bindings {
    pub(crate) projection_view_offset: ResourceHandle,
    pub(crate) vertices_offset: ResourceHandle,
    pub(crate) transforms_offset: ResourceHandle,
}

pub(crate) struct Renderer {
    // TODO: Make a material system
    materials: HashMap<String, Pipeline>,
    meshes: HashMap<String, Mesh>,
    // NOTE: Temporary
    renderables: Vec<RenderObject>,

    _projection_view_buffer: Buffer,

    _upload_semaphore: TimelineSemaphore,
    _upload_value: u64,

    _upload_command_buffer: CommandBuffer,
    _upload_command_pool: CommandPool,

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
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        swapchain: &Swapchain,
    ) -> CreationResult<Self> {
        let command_pool = CommandPool::new(instance, surface, device.clone())?;

        let mut command_buffers = Vec::new();
        let mut present_semaphores = Vec::new();
        let mut render_semaphores = Vec::new();
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let command_buffer = CommandBuffer::new(device.clone(), &command_pool)?;
            let present_semaphore = BinarySemaphore::new(device.clone())?;
            let render_semaphore = BinarySemaphore::new(device.clone())?;

            command_buffers.push(command_buffer);
            present_semaphores.push(present_semaphore);
            render_semaphores.push(render_semaphore);
        }

        let submit_semaphore = TimelineSemaphore::new(device.clone())?;

        let upload_command_pool = CommandPool::new(instance, surface, device.clone())?;
        let upload_command_buffer = CommandBuffer::new(device.clone(), &upload_command_pool)?;
        let upload_semaphore = TimelineSemaphore::new(device.clone())?;
        let mut upload_value = 0;

        ////////////////////////////////////////////////////////////////////////

        let mut materials = HashMap::new();

        let vertex_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_vs.spv")?;
        let fragment_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_ps.spv")?;
        let default_pipeline = Pipeline::new(
            device.clone(),
            descriptor_manager.clone(),
            swapchain,
            vertex_shader,
            fragment_shader,
        )?;

        materials.insert("default".to_string(), default_pipeline);

        ////////////////////////////////////////////////////////////////////////

        let mut meshes = HashMap::new();

        let triangle_vertices = vec![
            Vertex {
                position: Vec4f::new(1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(1.0, 0.0, 0.0, 1.0),
            },
            Vertex {
                position: Vec4f::new(0.0, -1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 0.0, 1.0, 1.0),
            },
            Vertex {
                position: Vec4f::new(-1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 1.0, 0.0, 1.0),
            },
        ];

        let triangle_mesh = Mesh::new(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            &upload_command_pool,
            &upload_command_buffer,
            &upload_semaphore,
            &mut upload_value,
            triangle_vertices,
        )?;

        meshes.insert("triangle".to_string(), triangle_mesh);

        let monkey_mesh = Mesh::load(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            &upload_command_pool,
            &upload_command_buffer,
            &upload_semaphore,
            &mut upload_value,
            "./assets/models/monkey_smooth.obj",
        )?;

        meshes.insert("monkey".to_string(), monkey_mesh);

        ////////////////////////////////////////////////////////////////////////

        // TODO: Abstract into camera struct and add dynamic update
        let projection_view_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        let camera_position = Vec3f::new(0.0, -1.0, -5.0);

        let mut view_matrix = Mat4x4f::identity();
        view_matrix.append_translation_mut(&camera_position);

        let projection_matrix =
            Mat4x4f::new_perspective(f32::to_radians(90.0), 1280.0 / 720.0, 0.1, 200.0);

        let projection_view = projection_matrix * view_matrix;
        Self::upload_buffer(
            device.clone(),
            allocator.clone(),
            &upload_command_pool,
            &upload_command_buffer,
            &upload_semaphore,
            &mut upload_value,
            &[projection_view],
            &projection_view_buffer,
        )
        .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

        let projection_view_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&projection_view_buffer);

        ////////////////////////////////////////////////////////////////////////

        let mut renderables = Vec::new();

        let monkey = RenderObject::new(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            &upload_command_pool,
            &upload_command_buffer,
            &upload_semaphore,
            &mut upload_value,
            "monkey",
            "default",
            [Mat4x4f::identity()].to_vec(),
            meshes["monkey"].vertex_buffer_handle(),
            projection_view_buffer_handle,
        )?;
        renderables.push(monkey);

        let mut transforms = Vec::new();
        for x in -20..=20 {
            for y in -20..=20 {
                let mut transform = Mat4x4f::identity();
                transform.append_translation_mut(&Vec3f::new(x as f32, 0.0, y as f32));
                transform.append_scaling_mut(0.18);
                transforms.push(transform);
            }
        }

        let triangle = RenderObject::new(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            &upload_command_pool,
            &upload_command_buffer,
            &upload_semaphore,
            &mut upload_value,
            "triangle",
            "default",
            transforms,
            meshes["triangle"].vertex_buffer_handle(),
            projection_view_buffer_handle,
        )?;
        renderables.push(triangle);

        Ok(Self {
            materials,
            meshes,
            renderables,

            _projection_view_buffer: projection_view_buffer,

            _upload_semaphore: upload_semaphore,
            _upload_value: upload_value,

            _upload_command_buffer: upload_command_buffer,
            _upload_command_pool: upload_command_pool,

            current_frame_id: 0,
            swapchain_image_index: 0,

            submit_semaphore,

            render_semaphores,
            present_semaphores,

            command_buffers,
            _command_pool: command_pool,

            descriptor_manager,
            device,
        })
    }

    #[allow(clippy::too_many_arguments)]
    pub(crate) fn begin(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        frame_id: u64,
        swapchain: &mut Swapchain,
    ) -> RuntimeResult<()> {
        self.current_frame_id = frame_id;
        self.submit_semaphore.wait_for(frame_id - 1)?;

        let side = self.current_frame_id % 2;

        let Some(index) = swapchain.acquire_next_image(
            window,
            instance,
            surface,
            &self.present_semaphores[side as usize])? else {
            return Ok(());
        };

        self.swapchain_image_index = index;

        self.command_buffers[side as usize].reset()?;
        self.command_buffers[side as usize].begin(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT)?;

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

        let render_area_extent = swapchain.extent();
        let render_area_offset = vk::Offset2D::builder().x(0).y(0);

        let render_area = vk::Rect2D::builder()
            .extent(render_area_extent)
            .offset(*render_area_offset);

        let color_attachment_info = vk::RenderingAttachmentInfoKHR::builder()
            .image_view(swapchain.image_views()[self.swapchain_image_index as usize])
            .image_layout(vk::ImageLayout::ATTACHMENT_OPTIMAL_KHR)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(color_clear);

        let color_attachments = [*color_attachment_info];

        let depth_attachment_info = vk::RenderingAttachmentInfoKHR::builder()
            .image_view(swapchain.depth_image_view())
            .image_layout(vk::ImageLayout::DEPTH_ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(depth_clear);

        let rendering_info = vk::RenderingInfoKHR::builder()
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

        Ok(())
    }

    pub(crate) fn end(&self, swapchain: &Swapchain) -> RuntimeResult<()> {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize].end_rendering();

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

        self.command_buffers[side as usize].end()?;

        Ok(())
    }

    pub(crate) fn submit(
        &self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) -> RuntimeResult<()> {
        let side = self.current_frame_id % 2;

        self.device.submit_render_queue(
            &self.command_buffers[side as usize],
            &self.present_semaphores[side as usize],
            &self.render_semaphores[side as usize],
            &self.submit_semaphore,
            self.current_frame_id,
        )?;

        swapchain.present_queue(
            window,
            instance,
            surface,
            self.device.present_queue(),
            &self.render_semaphores[side as usize],
            self.swapchain_image_index,
        )?;

        Ok(())
    }

    pub(crate) fn draw_objects(&self) {
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
                    material,
                    0,
                    &descriptor_sets,
                    &[],
                );

                last_material = current_material.to_string();
            }

            let material = &self.materials[&last_material];

            let bindings_offset = BindingsOffset::new(renderable.bindings_handle());
            self.command_buffers[side as usize].push_constants(
                material,
                vk::ShaderStageFlags::ALL,
                0,
                &bindings_offset,
            );

            let mesh = &self.meshes[renderable.mesh()];
            self.command_buffers[side as usize].draw(
                mesh.vertices().len() as u32,
                renderable.transforms().len() as u32,
                0,
                0,
            );
        }
    }

    pub(crate) fn resize(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) -> RuntimeResult<()> {
        swapchain.recreate(window, instance, surface)?;

        Ok(())
    }

    // NOTE: We pass everything for now so we can use it from everywhere

    // TODO: Move this logic
    fn immediate_submit<F>(
        device: Rc<Device>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        function: F,
    ) -> RuntimeResult<()>
    where
        F: FnOnce(&CommandBuffer),
    {
        upload_command_buffer.begin(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT)?;

        function(upload_command_buffer);

        upload_command_buffer.end()?;

        device.submit_upload_queue(upload_command_buffer, upload_semaphore, *upload_value)?;
        *upload_value += 1;
        upload_semaphore.wait_for(*upload_value)?;

        upload_command_pool.reset()?;

        Ok(())
    }

    // TODO: Move this logic
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn upload_buffer<T>(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        data: &[T],
        buffer: &Buffer,
    ) -> RuntimeResult<()>
    where
        T: Debug,
    {
        let buffer_size = mem::size_of_val(data);

        let staging_buffer = Buffer::new(
            device.clone(),
            allocator,
            buffer_size,
            vk::BufferUsageFlags::TRANSFER_SRC,
            MemoryLocation::CpuToGpu,
        )
        .map_err(|error| RuntimeError::CreationError(error, "staging buffer"))?;

        staging_buffer.set_data(data)?;

        Self::immediate_submit(
            device,
            upload_command_pool,
            upload_command_buffer,
            upload_semaphore,
            upload_value,
            |command_buffer| {
                let buffer_copy = vk::BufferCopy::builder()
                    .src_offset(0)
                    .dst_offset(0)
                    .size(buffer_size as u64);
                command_buffer.copy_buffer(&staging_buffer, buffer, &[*buffer_copy]);
            },
        )?;

        Ok(())
    }
}
