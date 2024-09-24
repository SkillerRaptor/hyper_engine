//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use hyper_math::Vec4;
use hyper_rhi::{
    buffer::{Buffer, BufferDescriptor, BufferUsage},
    command_list::CommandList,
    graphics_device::GraphicsDevice,
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    pipeline_layout::{PipelineLayout, PipelineLayoutDescriptor},
    render_pass::{DrawIndexedArguments, RenderPassDescriptor},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    surface::Surface,
};

use crate::{material::Material, mesh::Mesh, push_constants::ObjectPushConstants, scene::Scene};

pub struct Renderer {
    frame_index: u32,

    indices: Arc<dyn Buffer>,
    mesh: Arc<dyn Buffer>,
    _normals: Arc<dyn Buffer>,
    _positions: Arc<dyn Buffer>,
    material: Arc<dyn Buffer>,

    pipeline: Arc<dyn GraphicsPipeline>,
    _pipeline_layout: Arc<dyn PipelineLayout>,

    command_list: Arc<dyn CommandList>,

    graphics_device: Arc<dyn GraphicsDevice>,
}

impl Renderer {
    pub fn new(graphics_device: &Arc<dyn GraphicsDevice>) -> Self {
        let command_list = graphics_device.create_command_list();

        let pipeline_layout = graphics_device.create_pipeline_layout(&PipelineLayoutDescriptor {
            label: Some("Object Pipeline Layout"),
            push_constants_size: size_of::<ObjectPushConstants>(),
        });

        let vertex_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
            label: Some("Opaque Vertex Shader"),
            path: "./assets/shaders/opaque_shader.hlsl",
            entry_point: "vs_main",
            stage: ShaderStage::Vertex,
        });

        let fragment_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
            label: Some("Opaque Fragment Shader"),
            path: "./assets/shaders/opaque_shader.hlsl",
            entry_point: "fs_main",
            stage: ShaderStage::Fragment,
        });

        let pipeline = graphics_device.create_graphics_pipeline(&GraphicsPipelineDescriptor {
            label: Some("Opaque Graphics Pipeline"),
            layout: &pipeline_layout,
            vertex_shader: &vertex_shader,
            fragment_shader: &fragment_shader,
        });

        let material = graphics_device.create_buffer(&BufferDescriptor {
            label: Some("Material Buffer"),
            data: bytemuck::cast_slice(&[Material {
                base_color: Vec4::new(1.0, 0.0, 0.0, 1.0),
            }]),
            usage: BufferUsage::STORAGE,
        });

        let positions = graphics_device.create_buffer(&BufferDescriptor {
            label: Some("Positions Buffer"),
            data: bytemuck::cast_slice(&[
                Vec4::new(-0.5, -0.5, 0.0, 1.0),
                Vec4::new(0.5, -0.5, 0.0, 1.0),
                Vec4::new(0.5, 0.5, 0.0, 1.0),
                Vec4::new(-0.5, 0.5, 0.0, 1.0),
            ]),
            usage: BufferUsage::STORAGE,
        });

        let normals = graphics_device.create_buffer(&BufferDescriptor {
            label: Some("Normals Buffer"),
            data: bytemuck::cast_slice(&[Vec4::ZERO, Vec4::ZERO, Vec4::ZERO, Vec4::ZERO]),
            usage: BufferUsage::STORAGE,
        });

        let mesh = graphics_device.create_buffer(&BufferDescriptor {
            label: Some("Mesh Buffer"),
            data: bytemuck::cast_slice(&[Mesh {
                positions: positions.handle(),
                normals: normals.handle(),
                ..Default::default()
            }]),
            usage: BufferUsage::STORAGE,
        });

        let indices = graphics_device.create_buffer(&BufferDescriptor {
            label: Some("Index Buffer"),
            data: bytemuck::cast_slice(&[0_u32, 1_u32, 2_u32, 2_u32, 3_u32, 0_u32, 0, 0]),
            usage: BufferUsage::INDEX,
        });

        Self {
            frame_index: 1,

            indices,
            mesh,
            _normals: normals,
            _positions: positions,
            material,

            pipeline,
            _pipeline_layout: pipeline_layout,

            command_list,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub fn render(&mut self, mut surface: &mut Box<dyn Surface>, _scene: &Scene) {
        self.graphics_device
            .begin_frame(&mut surface, self.frame_index);

        let swapchain_texture = surface.current_texture();

        self.command_list.begin();

        {
            let render_pass = self.command_list.begin_render_pass(&RenderPassDescriptor {
                label: Some("Main Render Pass"),
                color_attachment: &swapchain_texture,
                depth_stencil_attachment: None,
            });

            render_pass.set_pipeline(&self.pipeline);
            render_pass.set_index_buffer(&self.indices);
            render_pass.set_push_constants(bytemuck::bytes_of(&ObjectPushConstants {
                mesh: self.mesh.handle(),
                material: self.material.handle(),
                ..Default::default()
            }));

            render_pass.draw_indexed(&DrawIndexedArguments {
                index_count: 6,
                instance_count: 1,
                first_index: 0,
                vertex_offset: 0,
                first_instance: 0,
            });
        }

        self.command_list.end();

        self.graphics_device.execute(&self.command_list);
        self.graphics_device.present(surface);

        self.frame_index += 1;
    }

    pub fn render_ui(&self) {}
}
