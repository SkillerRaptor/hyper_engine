//
// Copyright (c) 2022-2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{sync::Arc, time::Instant};

use bytemuck::NoUninit;
use hyper_math::Vec4;
use hyper_platform::{Window, WindowDescriptor};
use hyper_rhi::{
    buffer::{Buffer, BufferDescriptor, BufferUsage},
    commands::{
        descriptor::{Descriptor, DescriptorBuilder},
        render_pass::RenderPassDescriptor,
    },
    graphics_device::{GraphicsApi, GraphicsDevice, GraphicsDeviceDescriptor},
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    surface::{Surface, SurfaceDescriptor},
};
use raw_window_handle::{HasDisplayHandle, HasWindowHandle};

#[derive(Clone, Debug)]
pub struct EngineDescriptor {
    pub width: u32,
    pub height: u32,
    pub graphics_api: GraphicsApi,
    pub debug: bool,
}

pub struct Engine {
    frame_index: u32,

    opaque_descriptor: Descriptor,
    index_buffer: Arc<dyn Buffer>,
    vertex_buffer: Arc<dyn Buffer>,

    graphics_pipeline: Arc<dyn GraphicsPipeline>,
    surface: Box<dyn Surface>,
    graphics_device: Arc<dyn GraphicsDevice>,

    window: Window,
}

impl Engine {
    pub fn new(descriptor: &EngineDescriptor) -> Self {
        let start_time = Instant::now();

        // Assuming that the engine will run in editor-mode
        // FIXME: When adding game mode change the title to the game
        let title = if cfg!(debug_assertions) {
            "Hyper Engine (Debug Build)"
        } else {
            "Hyper Engine"
        };

        let window = Window::new(&WindowDescriptor {
            title: &title,
            width: descriptor.width,
            height: descriptor.height,
        })
        .unwrap();

        let graphics_device =
            hyper_rhi::graphics_device::create_graphics_device(&GraphicsDeviceDescriptor {
                graphics_api: descriptor.graphics_api,
                debug_mode: descriptor.debug,
                display_handle: window.display_handle().unwrap(),
            });

        let surface = graphics_device.create_surface(&SurfaceDescriptor {
            display_handle: window.display_handle().unwrap(),
            window_handle: window.window_handle().unwrap(),
            window_size: window.inner_size(),
        });

        let opaque_vertex_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
            path: "./assets/shaders/opaque_shader.hlsl",
            entry_point: "vs_main",
            stage: ShaderStage::Vertex,
        });

        let opaque_fragment_shader =
            graphics_device.create_shader_module(&ShaderModuleDescriptor {
                path: "./assets/shaders/opaque_shader.hlsl",
                entry_point: "fs_main",
                stage: ShaderStage::Fragment,
            });

        let graphics_pipeline =
            graphics_device.create_graphics_pipeline(&GraphicsPipelineDescriptor {
                vertex_shader: &opaque_vertex_shader,
                fragment_shader: &opaque_fragment_shader,
            });

        #[repr(C)]
        #[derive(Copy, Clone, NoUninit)]
        struct Vertex {
            position: Vec4,
            color: Vec4,
        }

        let vertex_buffer = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[
                Vertex {
                    position: Vec4::new(-0.5, -0.5, 0.0, 1.0),
                    color: Vec4::new(1.0, 0.0, 0.0, 1.0),
                },
                Vertex {
                    position: Vec4::new(0.5, -0.5, 0.0, 1.0),
                    color: Vec4::new(0.0, 1.0, 0.0, 1.0),
                },
                Vertex {
                    position: Vec4::new(0.5, 0.5, 0.0, 1.0),
                    color: Vec4::new(0.0, 0.0, 1.0, 1.0),
                },
                Vertex {
                    position: Vec4::new(-0.5, 0.5, 0.0, 1.0),
                    color: Vec4::new(1.0, 1.0, 1.0, 1.0),
                },
            ]),
            usage: BufferUsage::STORAGE,
        });

        let index_buffer = graphics_device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&[0_u32, 1_u32, 2_u32, 2_u32, 3_u32, 0_u32]),
            usage: BufferUsage::INDEX,
        });

        let opaque_descriptor = DescriptorBuilder::default()
            .read_buffer(0, &vertex_buffer)
            .build(&graphics_device);

        tracing::info!(
            "Engine initialized in {:.2} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Self {
            frame_index: 1,

            opaque_descriptor,
            index_buffer,
            vertex_buffer,

            graphics_pipeline,
            surface,
            graphics_device,

            window,
        }
    }

    pub fn run(&mut self) {
        // Fixed at 60 frames per second
        let mut _time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;

        while !self.window.close_requested() {
            let new_time = Instant::now();
            let frame_time = (new_time - current_time).as_secs_f32();
            current_time = new_time;

            accumulator += frame_time;

            self.window.process_events();

            if self.window.resized() {
                let size = self.window.inner_size();
                let width = size.x.max(1);
                let height = size.y.max(1);
                self.surface.resize(width, height);
            }

            while accumulator >= delta_time {
                // Update Fixed
                accumulator -= delta_time;
                _time += delta_time;
            }

            // Update

            // Render
            self.graphics_device
                .begin_frame(&mut self.surface, self.frame_index);

            let swapchain_texture = self.surface.current_texture();

            let mut command_encoder = self.graphics_device.create_command_encoder();

            {
                let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
                    texture: &swapchain_texture,
                });

                render_pass.bind_pipeline(&self.graphics_pipeline);
                render_pass.bind_descriptor(&self.opaque_descriptor);
                render_pass.bind_index_buffer(&self.index_buffer);
                render_pass.draw_indexed(6, 1, 0, 0, 0);
            }

            self.graphics_device.end_frame();

            self.graphics_device.submit(command_encoder.finish());
            self.graphics_device.present(&self.surface);

            self.frame_index += 1;
        }
    }
}
