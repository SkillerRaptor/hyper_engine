/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::time::Instant;

use hyper_platform::window::{Window, WindowDescriptor};
use hyper_rhi::{
    graphics_device::{GraphicsApi, GraphicsDevice, GraphicsDeviceDescriptor},
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    render_pass::RenderPassDescriptor,
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    surface::{Surface, SurfaceDescriptor},
};

#[derive(Clone, Copy, Debug)]
pub enum RendererApi {
    D3D12,
    Vulkan,
}

#[derive(Debug)]
pub struct EngineDescriptor {
    pub width: u32,
    pub height: u32,
    pub renderer: RendererApi,
    pub debug: bool,
}

pub struct Engine {
    // Rendering
    graphics_pipeline: GraphicsPipeline,
    surface: Surface,
    graphics_device: GraphicsDevice,

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

        let graphics_device = GraphicsDevice::new(&GraphicsDeviceDescriptor {
            // TODO: Don't hardcode and use CLI options
            graphics_api: match descriptor.renderer {
                RendererApi::D3D12 => GraphicsApi::D3D12,
                RendererApi::Vulkan => GraphicsApi::Vulkan,
            },
            debug_mode: descriptor.debug,
            window: &window,
        });

        let surface = graphics_device.create_surface(&SurfaceDescriptor { window: &window });

        let vertex_shader = graphics_device
            .create_shader_module(&ShaderModuleDescriptor {
                path: "./assets/shaders/vertex_shader.hlsl",
                entry: "main",
                stage: ShaderStage::Vertex,
            })
            .unwrap();

        let pixel_shader = graphics_device
            .create_shader_module(&ShaderModuleDescriptor {
                path: "./assets/shaders/pixel_shader.hlsl",
                entry: "main",
                stage: ShaderStage::Pixel,
            })
            .unwrap();

        let graphics_pipeline =
            graphics_device.create_graphics_pipeline(&GraphicsPipelineDescriptor {
                vertex_shader: &vertex_shader,
                pixel_shader: &pixel_shader,
            });

        log::info!(
            "Application initialized in {:.4} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Self {
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

            // Update
            while accumulator >= delta_time {
                // Update Fixed
                accumulator -= delta_time;
                _time += delta_time;
            }

            // Normal Update

            // Render
            let swapchain_texture = self.surface.current_texture();

            let command_list = self.graphics_device.create_command_list();

            {
                let render_pass = command_list.begin_render_pass(&RenderPassDescriptor {
                    texture: &swapchain_texture,
                });

                render_pass.bind_pipeline(&self.graphics_pipeline);
                render_pass.draw(3, 1, 0, 0);
            }

            self.graphics_device.execute_commands(&command_list);

            self.surface.present();
        }
    }
}
