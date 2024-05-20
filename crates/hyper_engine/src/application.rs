/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{borrow::Cow, num::NonZeroU32, time::Instant};

use hyper_platform::window::{self, Window, WindowDescriptor};
use hyper_rhi::{
    graphics_device::{GraphicsApi, GraphicsDevice, GraphicsDeviceDescriptor},
    render_pass::RenderPassDescriptor,
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    shader_module::{ShaderModuleDescriptor, ShaderModuleError, ShaderStage},
    surface::{Surface, SurfaceDescriptor},
};
use thiserror::Error;

use crate::game::Game;

#[derive(Debug, Error)]
pub enum ApplicationError {
    #[error(transparent)]
    Window(#[from] window::Error),

    #[error("failed to create shader module")]
    ShaderModuleCreation(#[from] ShaderModuleError),
}

#[derive(Debug)]
pub struct ApplicationDescriptor<'a> {
    pub title: &'a str,
    pub width: NonZeroU32,
    pub height: NonZeroU32,
    pub resizable: bool,
}

pub struct Application {
    // Rendering
    graphics_pipeline: GraphicsPipeline,
    surface: Surface,
    graphics_device: GraphicsDevice,

    window: Window,
    game: Box<dyn Game>,
}

impl Application {
    pub fn new(
        game: Box<dyn Game>,
        descriptor: ApplicationDescriptor,
    ) -> Result<Self, ApplicationError> {
        let start_time = Instant::now();

        let title = if cfg!(debug_assertions) {
            Cow::from(format!("{} (Debug Build)", descriptor.title))
        } else {
            Cow::from(descriptor.title)
        };

        let window = Window::new(&WindowDescriptor {
            title: &title,
            width: descriptor.width,
            height: descriptor.height,
            resizable: descriptor.resizable,
        })?;

        let graphics_device = GraphicsDevice::new(&GraphicsDeviceDescriptor {
            // TODO: Don't hardcode and use CLI options
            graphics_api: GraphicsApi::D3D12,
            debug_mode: cfg!(debug_assertions),
            window: &window,
        });

        let surface = graphics_device.create_surface(&SurfaceDescriptor { window: &window });

        let vertex_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
            path: "./assets/shaders/vertex_shader.hlsl",
            entry: "main",
            stage: ShaderStage::Vertex,
        })?;

        let pixel_shader = graphics_device.create_shader_module(&ShaderModuleDescriptor {
            path: "./assets/shaders/pixel_shader.hlsl",
            entry: "main",
            stage: ShaderStage::Pixel,
        })?;

        let graphics_pipeline = graphics_device.create_graphics_pipeline(&GraphicsPipelineDescriptor {
            vertex_shader: &vertex_shader,
            pixel_shader: &pixel_shader,
        });

        log::info!(
            "Application initialized in {:.4} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Ok(Self {
            graphics_pipeline,
            surface,
            graphics_device,

            window,
            game,
        })
    }

    pub fn run(&mut self) {
        // Fixed at 60 frames per second
        let mut time = 0.0;
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
                self.game.update_fixed(delta_time, time);
                accumulator -= delta_time;
                time += delta_time;
            }

            self.game.update();

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
