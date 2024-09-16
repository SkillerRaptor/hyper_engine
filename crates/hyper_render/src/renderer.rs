//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use hyper_math::Vec4;
use hyper_rhi::{
    buffer::{Buffer, BufferDescriptor, BufferUsage},
    commands::render_pass::RenderPassDescriptor,
    graphics_device::GraphicsDevice,
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    pipeline_layout::{PipelineLayout, PipelineLayoutDescriptor},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    surface::Surface,
};

use crate::{material::Material, mesh::Mesh, push_constants::ObjectPushConstants, scene::Scene};

pub struct Renderer {
    frame_index: u32,

    indices: Arc<dyn Buffer>,
    mesh: Arc<dyn Buffer>,
    normals: Arc<dyn Buffer>,
    positions: Arc<dyn Buffer>,
    material: Arc<dyn Buffer>,

    opaque_pipeline: Arc<dyn GraphicsPipeline>,
    object_pipeline_layout: Arc<dyn PipelineLayout>,

    graphics_device: Arc<dyn GraphicsDevice>,
}

impl Renderer {
    pub fn new(graphics_device: &Arc<dyn GraphicsDevice>) -> Self {
        let object_pipeline_layout =
            graphics_device.create_pipeline_layout(&PipelineLayoutDescriptor {
                push_constants_size: size_of::<ObjectPushConstants>(),
            });

        let opaque_pipeline = {
            let vertex_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
                path: "./assets/shaders/opaque_shader.hlsl",
                entry_point: "vs_main",
                stage: ShaderStage::Vertex,
            });

            let fragment_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
                path: "./assets/shaders/opaque_shader.hlsl",
                entry_point: "fs_main",
                stage: ShaderStage::Fragment,
            });

            graphics_device.create_graphics_pipeline(&GraphicsPipelineDescriptor {
                layout: &object_pipeline_layout,
                vertex_shader: &vertex_shader,
                fragment_shader: &fragment_shader,
            })
        };

        let material = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[Material {
                base_color: Vec4::new(1.0, 0.0, 0.0, 1.0),
            }]),
            usage: BufferUsage::STORAGE,
        });

        let positions = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[
                Vec4::new(-0.5, -0.5, 0.0, 1.0),
                Vec4::new(0.5, -0.5, 0.0, 1.0),
                Vec4::new(0.5, 0.5, 0.0, 1.0),
                Vec4::new(-0.5, 0.5, 0.0, 1.0),
            ]),
            usage: BufferUsage::STORAGE,
        });

        let normals = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[Vec4::ZERO, Vec4::ZERO, Vec4::ZERO, Vec4::ZERO]),
            usage: BufferUsage::STORAGE,
        });

        let mesh = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[Mesh {
                positions: positions.handle(),
                normals: normals.handle(),
                ..Default::default()
            }]),
            usage: BufferUsage::STORAGE,
        });

        let indices = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[0_u32, 1_u32, 2_u32, 2_u32, 3_u32, 0_u32, 0, 0]),
            usage: BufferUsage::INDEX,
        });

        Self {
            frame_index: 1,

            indices,
            mesh,
            normals,
            positions,
            material,

            opaque_pipeline,
            object_pipeline_layout,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub fn render(&mut self, mut surface: &mut Box<dyn Surface>, scene: &Scene) {
        self.graphics_device
            .begin_frame(&mut surface, self.frame_index);

        let swapchain_texture = surface.current_texture();

        let mut command_encoder = self.graphics_device.create_command_encoder();

        {
            let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
                texture: &swapchain_texture,
            });

            render_pass.bind_pipeline(&self.opaque_pipeline);

            render_pass.bind_index_buffer(&self.indices);
            render_pass.push_constants(bytemuck::bytes_of(&ObjectPushConstants {
                mesh: self.mesh.handle(),
                material: self.material.handle(),
                ..Default::default()
            }));
            render_pass.draw_indexed(6, 1, 0, 0, 0);
        }

        self.graphics_device.end_frame();

        self.graphics_device.submit(command_encoder.finish());
        self.graphics_device.present(surface);

        self.frame_index += 1;
    }

    pub fn render_ui(&self) {}
}

impl Drop for Renderer {
    fn drop(&mut self) {
        self.graphics_device.wait_idle();
    }
}
