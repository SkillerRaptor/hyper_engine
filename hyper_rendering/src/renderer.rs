/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator,
    binary_semaphore::BinarySemaphore,
    buffer::Buffer,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationError, CreationResult, RuntimeResult},
    instance::Instance,
    mesh::{Mesh, Vertex},
    pipeline::{BindingsOffset, Pipeline},
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
use std::{
    cell::RefCell,
    mem,
    rc::Rc,
    sync::{Arc, Mutex},
};

// NOTE: Temporary
#[repr(C)]
#[derive(Clone, Copy, Debug)]
struct Bindings {
    projection_view_offset: ResourceHandle,
    vertices_offset: ResourceHandle,
    transforms_offset: ResourceHandle,
}

pub(crate) struct Renderer {
    triangle_mesh: Mesh,
    triangle_pipeline: Pipeline,

    _transform_buffer: Buffer,
    _bindings_buffer: Buffer,
    bindings_buffer_handle: ResourceHandle,

    current_frame_id: u64,
    swapchain_image_index: u32,

    submit_semaphore: TimelineSemaphore,

    render_semaphores: Vec<BinarySemaphore>,
    present_semaphores: Vec<BinarySemaphore>,

    command_buffers: Vec<CommandBuffer>,
    _command_pool: CommandPool,

    descriptor_manager: Rc<RefCell<DescriptorManager>>,
    device: Arc<Device>,
}

impl Renderer {
    const FRAMES_IN_FLIGHT: u32 = 2;

    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Arc<Device>,
        allocator: Arc<Mutex<Allocator>>,
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

        ////////////////////////////////////////////////////////////////////////

        let vertex_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_vs.spv")?;
        let fragment_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_ps.spv")?;
        let triangle_pipeline = Pipeline::new(
            device.clone(),
            descriptor_manager.clone(),
            swapchain,
            vertex_shader,
            fragment_shader,
        )?;

        let triangle_vertices = vec![
            Vertex {
                position: Vec4f::new(1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(1.0, 0.0, 0.0, 1.0),
            },
            Vertex {
                position: Vec4f::new(-1.0, 1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 1.0, 0.0, 1.0),
            },
            Vertex {
                position: Vec4f::new(0.0, -1.0, 0.5, 1.0),
                normal: Vec4f::default(),
                color: Vec4f::new(0.0, 0.0, 1.0, 1.0),
            },
        ];

        let triangle_mesh = Mesh::new(
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            triangle_vertices,
        )?;

        // TODO: Abstract into camera struct and add dynamic update

        let projection_view_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        let camera_position = Vec3f::new(0.0, 0.0, -2.0);

        let view_matrix = Mat4x4f::identity().append_translation(&camera_position);

        let projection_matrix =
            Mat4x4f::new_perspective(f32::to_radians(90.0), 1280.0 / 720.0, 0.1, 200.0);

        let projection_view = projection_matrix * view_matrix;
        projection_view_buffer
            .set_data(&[projection_view])
            .map_err(|error| {
                CreationError::RuntimeError(Box::new(error), "set data for transform buffer")
            })?;

        let projection_view_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&projection_view_buffer);

        let transform_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        let transform = Mat4x4f::identity();
        transform_buffer.set_data(&[transform]).map_err(|error| {
            CreationError::RuntimeError(Box::new(error), "set data for transform buffer")
        })?;

        let transforms_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&transform_buffer);

        let bindings_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Bindings>(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        let bindings = Bindings {
            projection_view_offset: projection_view_buffer_handle,
            vertices_offset: triangle_mesh.vertex_buffer_handle(),
            transforms_offset: transforms_buffer_handle,
        };
        bindings_buffer.set_data(&[bindings]).map_err(|error| {
            CreationError::RuntimeError(Box::new(error), "set data for bindings buffer")
        })?;

        let bindings_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&bindings_buffer);

        Ok(Self {
            triangle_pipeline,
            triangle_mesh,

            _transform_buffer: transform_buffer,
            _bindings_buffer: bindings_buffer,
            bindings_buffer_handle,

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

        let clear_value = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.12941, 0.06275, 0.13725, 1.0],
            },
        };

        self.triangle_pipeline.begin_rendering(
            swapchain,
            &self.command_buffers[side as usize],
            swapchain.images()[self.swapchain_image_index as usize],
            swapchain.image_views()[self.swapchain_image_index as usize],
            clear_value,
        );

        self.triangle_pipeline
            .bind(&self.command_buffers[side as usize]);

        let descriptor_sets = self
            .descriptor_manager
            .borrow()
            .descriptor_sets()
            .iter()
            .map(|descriptor_set| descriptor_set.handle())
            .collect::<Vec<_>>();
        self.command_buffers[side as usize].bind_descriptor_sets(
            vk::PipelineBindPoint::GRAPHICS,
            &self.triangle_pipeline,
            0,
            &descriptor_sets,
            &[],
        );

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

        self.triangle_pipeline.end_rendering(
            &self.command_buffers[side as usize],
            swapchain.images()[self.swapchain_image_index as usize],
        );

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

        self.device.submit_queue(
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

    pub(crate) fn draw(&self) {
        // TODO: Draw scene and not a triangle

        let side = self.current_frame_id % 2;

        let bindings_offset = BindingsOffset::new(self.bindings_buffer_handle);

        self.command_buffers[side as usize].push_constants(
            &self.triangle_pipeline,
            vk::ShaderStageFlags::ALL,
            0,
            &bindings_offset,
        );

        self.command_buffers[side as usize].draw(
            self.triangle_mesh.vertices().len() as u32,
            1,
            0,
            0,
        );
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
}
