/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocation, Allocator, MemoryLocation},
    bindings::GuiBindings,
    buffer::Buffer,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationError, CreationResult, RuntimeResult},
    graphics_pipelines::{
        ColorBlendAttachmentStateCreateInfo, ColorBlendStateCreateInfo,
        DepthStencilStateCreateInfo, GraphicsPipeline, GraphicsPipelineCreateInfo,
        InputAssemblyCreateInfo, RasterizationStateCreateInfo,
    },
    pipeline_layout::PipelineLayout,
    renderer::Renderer,
    resource_handle::ResourceHandle,
    shader::Shader,
    swapchain::Swapchain,
    upload_manager::UploadManager,
};

use hyper_math::vector::Vec4f;
use hyper_platform::{event_loop::EventLoop, window::Window};

use ash::vk;
use egui::{
    epaint::Primitive, ClippedPrimitive, Context, FontDefinitions, FullOutput, Style, TextureId,
};
use egui_winit::State;
use std::{cell::RefCell, collections::HashMap, mem, rc::Rc};
use winit::event::WindowEvent;

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct Vertex {
    pub(crate) position: Vec4f,
    pub(crate) color: Vec4f,
    pub(crate) uv: Vec4f,
}

struct Texture {
    image: vk::Image,
    image_view: vk::ImageView,
    sampler: vk::Sampler,
    allocation: Allocation,
    handle: ResourceHandle,
}

pub(crate) struct EguiIntegration {
    textures: HashMap<TextureId, Texture>,

    upload_manager: Rc<RefCell<UploadManager>>,
    descriptor_manager: Rc<RefCell<DescriptorManager>>,

    vertex_buffer: Buffer,
    vertex_buffer_handle: ResourceHandle,

    index_buffer: Buffer,

    bindings_buffer: Buffer,
    bindings_handle: ResourceHandle,

    pipeline: GraphicsPipeline,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,

    egui_winit: State,
    egui_context: Context,
}

impl EguiIntegration {
    pub(crate) fn new(
        event_loop: &EventLoop,
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        pipeline_layout: &PipelineLayout,
        swapchain: &Swapchain,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_manager: Rc<RefCell<UploadManager>>,
    ) -> CreationResult<Self> {
        let vertex_shader = Shader::new(device.clone(), "./assets/shaders/compiled/gui_vs.spv")?;
        let fragment_shader = Shader::new(device.clone(), "./assets/shaders/compiled/gui_fs.spv")?;

        let pipeline = GraphicsPipeline::new(
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
                    cull_mode: vk::CullModeFlags::NONE,

                    depth_bias_enable: false,
                    ..Default::default()
                },
                depth_stencil_state: DepthStencilStateCreateInfo {
                    depth_test_enable: false,
                    depth_write_enable: false,

                    depth_bounds_test_enable: false,
                    ..Default::default()
                },
                color_blend_attachment_state: ColorBlendAttachmentStateCreateInfo {
                    blend_enable: true,

                    src_color_blend_factor: vk::BlendFactor::ONE,
                    dst_color_blend_factor: vk::BlendFactor::ONE_MINUS_SRC_ALPHA,
                    color_blend_op: vk::BlendOp::ADD,

                    color_write_mask: vk::ColorComponentFlags::RGBA,
                    ..Default::default()
                },
                color_blend_state: ColorBlendStateCreateInfo {
                    logic_op_enable: false,
                    ..Default::default()
                },
            },
        )?;

        ////////////////////////////////////////////////////////////////////////

        let vertex_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Vertex>() * 1024 * 1024,
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        let vertex_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&vertex_buffer);

        ////////////////////////////////////////////////////////////////////////

        let index_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<u32>() * 1024 * 1024,
            vk::BufferUsageFlags::INDEX_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        ////////////////////////////////////////////////////////////////////////

        let bindings_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<GuiBindings>(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        let resource_handles = vec![vertex_buffer_handle, ResourceHandle::new(0)];

        upload_manager
            .borrow_mut()
            .upload_buffer(&resource_handles, &bindings_buffer)
            .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

        let bindings_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&bindings_buffer);

        ////////////////////////////////////////////////////////////////////////

        let egui_context = Context::default();
        egui_context.set_fonts(FontDefinitions::default());
        egui_context.set_style(Style::default());

        let egui_winit = State::new(&event_loop.internal());

        Ok(Self {
            textures: HashMap::new(),

            upload_manager,
            descriptor_manager,

            vertex_buffer,
            vertex_buffer_handle,

            index_buffer,

            bindings_buffer,
            bindings_handle,

            pipeline,

            allocator,
            device,

            egui_winit,
            egui_context,
        })
    }

    pub(crate) fn handle_event(&mut self, winit_event: &WindowEvent<'_>) {
        let _ = self.egui_winit.on_event(&self.egui_context, winit_event);
    }

    pub(crate) fn begin_gui(&mut self, window: &Window) {
        let raw_input = self.egui_winit.take_egui_input(window.internal());
        self.egui_context.begin_frame(raw_input);
    }

    pub(crate) fn end_gui(&mut self, window: &Window) -> FullOutput {
        let output = self.egui_context.end_frame();

        self.egui_winit.handle_platform_output(
            window.internal(),
            &self.egui_context,
            output.platform_output.clone(),
        );

        output
    }

    pub(crate) fn submit_gui(
        &mut self,
        window: &Window,
        swapchain: &mut Swapchain,
        pipeline_layout: &PipelineLayout,
        renderer: &mut Renderer,
        output: FullOutput,
    ) -> RuntimeResult<()> {
        let clipped_meshes = self.egui_context.tessellate(output.shapes);

        for (id, image_delta) in output.textures_delta.set {
            if self.textures.contains_key(&id) {
                continue;
            }

            // TODO: Upload texture
            let data: Vec<u8> = match &image_delta.image {
                egui::ImageData::Color(image) => {
                    assert_eq!(
                        image.width() * image.height(),
                        image.pixels.len(),
                        "Mismatch between texture size and texel count"
                    );
                    image
                        .pixels
                        .iter()
                        .flat_map(|color| color.to_array())
                        .collect()
                }
                egui::ImageData::Font(image) => image
                    .srgba_pixels(None)
                    .flat_map(|color| color.to_array())
                    .collect(),
            };

            let staging_buffer = Buffer::new(
                self.device.clone(),
                self.allocator.clone(),
                data.len(),
                vk::BufferUsageFlags::TRANSFER_SRC,
                MemoryLocation::CpuToGpu,
            )
            .unwrap();

            staging_buffer.set_data(&data)?;

            let extent = vk::Extent3D::builder()
                .width(image_delta.image.width() as u32)
                .height(image_delta.image.height() as u32)
                .depth(1);

            let format = vk::Format::R8G8B8A8_SRGB;

            let create_info = vk::ImageCreateInfo::builder()
                .image_type(vk::ImageType::TYPE_2D)
                .format(format)
                .extent(*extent)
                .mip_levels(1)
                .array_layers(1)
                .samples(vk::SampleCountFlags::TYPE_1)
                .tiling(vk::ImageTiling::OPTIMAL)
                .usage(vk::ImageUsageFlags::SAMPLED | vk::ImageUsageFlags::TRANSFER_DST)
                .sharing_mode(vk::SharingMode::EXCLUSIVE)
                .queue_family_indices(&[])
                .initial_layout(vk::ImageLayout::UNDEFINED);

            let handle = unsafe {
                self.device
                    .handle()
                    .create_image(&create_info, None)
                    .unwrap()
            };

            let memory_requirements =
                unsafe { self.device.handle().get_image_memory_requirements(handle) };

            let allocation = self
                .allocator
                .borrow_mut()
                .allocate(MemoryLocation::GpuOnly, memory_requirements)?;

            unsafe {
                self.device
                    .handle()
                    .bind_image_memory(handle, allocation.0.memory(), allocation.0.offset())
                    .unwrap();
            }

            self.upload_manager
                .borrow_mut()
                .upload_texture(&staging_buffer, handle, *extent)?;

            let subsource_range = vk::ImageSubresourceRange::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let view_create_info = vk::ImageViewCreateInfo::builder()
                .image(handle)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(format)
                .components(vk::ComponentMapping::default())
                .subresource_range(*subsource_range);

            let view = unsafe {
                self.device
                    .handle()
                    .create_image_view(&view_create_info, None)
                    .unwrap()
            };

            let sampler_create_info = vk::SamplerCreateInfo::builder()
                .mag_filter(vk::Filter::LINEAR)
                .min_filter(vk::Filter::LINEAR)
                .address_mode_u(vk::SamplerAddressMode::CLAMP_TO_EDGE)
                .address_mode_v(vk::SamplerAddressMode::CLAMP_TO_EDGE)
                .address_mode_w(vk::SamplerAddressMode::CLAMP_TO_EDGE)
                .anisotropy_enable(false)
                .mipmap_mode(vk::SamplerMipmapMode::LINEAR)
                .min_lod(0.0)
                .max_lod(vk::LOD_CLAMP_NONE);

            let sampler = unsafe {
                self.device
                    .handle()
                    .create_sampler(&sampler_create_info, None)
                    .unwrap()
            };

            let combined_image_sampler_handle = self
                .descriptor_manager
                .borrow_mut()
                .allocate_combined_image_sampler_handle(
                    sampler,
                    view,
                    vk::ImageLayout::SHADER_READ_ONLY_OPTIMAL,
                );

            let resource_handles = vec![self.vertex_buffer_handle, combined_image_sampler_handle];

            self.upload_manager
                .borrow_mut()
                .upload_buffer(&resource_handles, &self.bindings_buffer)?;

            self.descriptor_manager
                .borrow_mut()
                .update_buffer_handle(self.bindings_handle, &self.bindings_buffer);

            let texture = Texture {
                image: handle,
                image_view: view,
                sampler,
                allocation,
                handle: combined_image_sampler_handle,
            };

            self.textures.insert(id, texture);
        }

        let mut vertices = Vec::new();
        let mut indices = Vec::new();
        for ClippedPrimitive { primitive, .. } in &clipped_meshes {
            let mesh = match primitive {
                Primitive::Mesh(mesh) => mesh,
                Primitive::Callback(_) => todo!(),
            };

            if mesh.vertices.is_empty() || mesh.indices.is_empty() {
                continue;
            }

            vertices.extend_from_slice(&mesh.vertices);
            indices.extend_from_slice(&mesh.indices);
        }

        if !vertices.is_empty() {
            let vertices = vertices
                .iter()
                .map(|vertex| Vertex {
                    position: Vec4f::new(
                        vertex.pos.x,
                        window.framebuffer_size().1 as f32 - vertex.pos.y,
                        0.0,
                        0.0,
                    ),
                    color: Vec4f::new(
                        vertex.color.r() as f32 / 255.0,
                        vertex.color.g() as f32 / 255.0,
                        vertex.color.b() as f32 / 255.0,
                        vertex.color.a() as f32 / 255.0,
                    ),
                    uv: Vec4f::new(vertex.uv.x, vertex.uv.y, 0.0, 0.0),
                })
                .collect::<Vec<_>>();

            self.upload_manager
                .borrow_mut()
                .upload_buffer::<Vertex>(&vertices, &self.vertex_buffer)?;

            self.descriptor_manager
                .borrow_mut()
                .update_buffer_handle(self.vertex_buffer_handle, &self.vertex_buffer);
        }

        if !indices.is_empty() {
            self.upload_manager
                .borrow_mut()
                .upload_buffer::<u32>(&indices, &self.index_buffer)?;
        }

        renderer.begin_rendering_gui(swapchain);

        renderer.bind_pipeline(&self.pipeline, pipeline_layout);
        renderer.bind_index_buffer(&self.index_buffer);

        renderer.push_constants(pipeline_layout, self.bindings_handle);

        let mut vertex_base = 0;
        let mut index_base = 0;
        for ClippedPrimitive {
            clip_rect,
            primitive,
        } in clipped_meshes
        {
            let mesh = match primitive {
                Primitive::Mesh(mesh) => mesh,
                Primitive::Callback(_) => todo!(),
            };

            if mesh.vertices.is_empty() || mesh.indices.is_empty() {
                continue;
            }

            let min = clip_rect.min;
            let min = egui::Pos2 { x: min.x, y: min.y };
            let min = egui::Pos2 {
                x: f32::clamp(min.x, 0.0, swapchain.extent().width as f32),
                y: f32::clamp(min.y, 0.0, swapchain.extent().height as f32),
            };
            let max = clip_rect.max;
            let max = egui::Pos2 { x: max.x, y: max.y };
            let max = egui::Pos2 {
                x: f32::clamp(max.x, min.x, swapchain.extent().width as f32),
                y: f32::clamp(max.y, min.y, swapchain.extent().height as f32),
            };

            let offset = vk::Offset2D::builder()
                .x(min.x.round() as i32)
                .y(min.y.round() as i32);

            let extent = vk::Extent2D::builder()
                .width((max.x.round() - min.x) as u32)
                .height((max.y.round() - min.y) as u32);
            renderer.set_scissor(*offset, *extent);

            // TODO: Bind texture

            renderer.draw_indexed(mesh.indices.len() as u32, 1, index_base, vertex_base, 0);

            vertex_base += mesh.vertices.len() as i32;
            index_base += mesh.indices.len() as u32;
        }

        renderer.end_rendering();

        Ok(())
    }

    pub(crate) fn context(&self) -> &Context {
        &self.egui_context
    }
}
