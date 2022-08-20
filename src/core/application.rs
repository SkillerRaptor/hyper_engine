/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::rendering::context::RenderContext;

use colored::Colorize;
use log::{error, info};
use winit::{dpi, error, event, event_loop, platform::run_return::EventLoopExtRunReturn, window};

pub enum ApplicationError {
    IoError(std::io::Error),
    LogError(log::SetLoggerError),
    OsError(error::OsError),
}

impl std::fmt::Display for ApplicationError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ApplicationError::IoError(error) => {
                write!(formatter, "{}", error)
            }
            ApplicationError::LogError(error) => {
                write!(formatter, "{}", error)
            }
            ApplicationError::OsError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<std::io::Error> for ApplicationError {
    fn from(error: std::io::Error) -> Self {
        ApplicationError::IoError(error)
    }
}

impl From<log::SetLoggerError> for ApplicationError {
    fn from(error: log::SetLoggerError) -> Self {
        ApplicationError::LogError(error)
    }
}

impl From<error::OsError> for ApplicationError {
    fn from(error: error::OsError) -> Self {
        ApplicationError::OsError(error)
    }
}

pub struct Application {
    minimized: bool,
    resized: bool,
    destroyed: bool,
    render_context: RenderContext,
    window: window::Window,
    event_loop: event_loop::EventLoop<()>,
}

impl Application {
    pub fn new() -> Self {
        if let Err(error) = Self::setup_logger() {
            error!("Failed to create logger: {}", error);
            std::process::exit(1);
        }

        let event_loop = event_loop::EventLoop::new();
        let window = match Self::create_window(&event_loop, "HyperEngine", 1280, 720) {
            Ok(window) => window,
            Err(error) => {
                error!("Failed to create window: {}", error);
                std::process::exit(1);
            }
        };

        let render_context = match RenderContext::new(&window) {
            Ok(render_context) => render_context,
            Err(error) => {
                error!("Failed to create render context: {}", error);
                std::process::exit(1);
            }
        };

        Self {
            minimized: false,
            resized: false,
            destroyed: false,
            render_context,
            window,
            event_loop,
        }
    }

    fn create_window(
        event_loop: &event_loop::EventLoop<()>,
        title: &str,
        width: u32,
        height: u32,
    ) -> Result<window::Window, ApplicationError> {
        let window = window::WindowBuilder::new()
            .with_title(title)
            .with_inner_size(dpi::LogicalSize::new(width, height))
            .build(&event_loop)?;

        info!(
            "Created window '{}' ({}x{})",
            title,
            window.inner_size().width,
            window.inner_size().height
        );

        Ok(window)
    }

    pub fn run(&mut self) {
        let mut fps: u16 = 0;
        let mut last_frame = std::time::Instant::now();
        let mut last_fps_frame = std::time::Instant::now();
        self.event_loop.run_return(|event, _, control_flow| {
            *control_flow = event_loop::ControlFlow::Poll;

            match event {
                event::Event::WindowEvent { event, window_id } if window_id == self.window.id() => {
                    match event {
                        event::WindowEvent::Resized(size) => {
                            if size.width == 0 || size.height == 0 {
                                self.minimized = true;
                            } else {
                                self.minimized = false;
                                self.resized = true;
                            }
                        }

                        event::WindowEvent::CloseRequested => {
                            *control_flow = event_loop::ControlFlow::Exit;
                            self.destroyed = true;
                        }
                        _ => (),
                    }
                }

                event::Event::MainEventsCleared => {
                    self.window.request_redraw();
                }

                event::Event::RedrawRequested(_) if !self.destroyed && !self.minimized => {
                    let current_frame = std::time::Instant::now();
                    let delta_time = current_frame.duration_since(last_frame).as_secs_f64();

                    while current_frame.duration_since(last_fps_frame).as_secs_f64() >= 1.0 {
                        self.window.set_title(
                            format!("HyperEngine (Delta Time: {}, FPS: {})", delta_time, fps)
                                .as_str(),
                        );
                        fps = 0;
                        last_fps_frame = current_frame;
                    }

                    // NOTE: Update

                    // NOTE: Render
                    if let Err(error) = self.render_context.begin_frame(&self.window) {
                        error!("Failed to begin frame: {}", error);
                        std::process::exit(1);
                    }

                    self.render_context.draw_triangle(&self.window);

                    if let Err(error) = self.render_context.end_frame() {
                        error!("Failed to end frame: {}", error);
                        std::process::exit(1);
                    }

                    if let Err(error) = self.render_context.submit(&self.window, &mut self.resized)
                    {
                        error!("Failed to submit: {}", error);
                        std::process::exit(1);
                    }

                    fps += 1;
                    last_frame = current_frame;
                }
                _ => (),
            };
        });
    }

    fn setup_logger() -> Result<(), ApplicationError> {
        let logs_folder = "./logs/";
        if !std::path::Path::new(logs_folder).exists() {
            std::fs::create_dir(logs_folder)?;
        }

        fern::Dispatch::new()
            .level(log::LevelFilter::Trace)
            .chain(
                fern::Dispatch::new()
                    .format(|out, message, record| {
                        let time = chrono::Local::now().format("%H:%M:%S");
                        let level = match record.level() {
                            log::Level::Error => "error".red(),
                            log::Level::Warn => "warn".bright_yellow(),
                            log::Level::Info => "info".green(),
                            log::Level::Debug => "debug".cyan(),
                            log::Level::Trace => "trace".purple(),
                        };

                        out.finish(format_args!("{} | {}: {}", time, level, message))
                    })
                    .chain(std::io::stdout()),
            )
            .chain(
                fern::Dispatch::new()
                    .format(|out, message, record| {
                        let time = chrono::Local::now().format("%H:%M:%S");
                        let level = match record.level() {
                            log::Level::Error => "error",
                            log::Level::Warn => "warn",
                            log::Level::Info => "info",
                            log::Level::Debug => "debug",
                            log::Level::Trace => "trace",
                        };

                        out.finish(format_args!("{} | {}: {}", time, level, message))
                    })
                    .chain(fern::log_file(format!(
                        "logs/hyper_engine_{}.log",
                        chrono::Local::now().format("%d-%m-%Y_%H-%M-%S")
                    ))?),
            )
            .apply()?;

        Ok(())
    }
}
