/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use colored::Colorize;
use log::error;
use winit::{event, event_loop, platform::run_return::EventLoopExtRunReturn};

use crate::core::window::Window;
use crate::rendering::context::RenderContext;

pub enum ApplicationError {
    IoError(std::io::Error),
    LogError(log::SetLoggerError),
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

pub struct Application {
    minimized: bool,
    resized: bool,
    destroyed: bool,
    render_context: RenderContext,
    window: Window,
}

impl Application {
    pub fn new() -> Self {
        if let Err(error) = Self::setup_logger() {
            error!("Failed to create logger: {}", error);
            std::process::exit(1);
        }

        let window = match Window::new("HyperEngine", 1280, 720) {
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
        }
    }

    pub fn run(&mut self) {
        self.window.event_loop.run_return(|event, _, control_flow| {
            *control_flow = event_loop::ControlFlow::Poll;

            match event {
                event::Event::WindowEvent {
                    event: event::WindowEvent::Resized(size),
                    ..
                } => {
                    if size.width == 0 || size.height == 0 {
                        self.minimized = true;
                    } else {
                        self.minimized = false;
                        self.resized = true;
                    }
                }
                event::Event::WindowEvent {
                    event: event::WindowEvent::CloseRequested,
                    ..
                } => {
                    self.destroyed = true;
                    *control_flow = event_loop::ControlFlow::Exit;

                    unsafe {
                        self.render_context
                            .device
                            .device
                            .device_wait_idle()
                            .unwrap();
                    }
                }
                event::Event::MainEventsCleared => {
                    // TODO: Update

                    self.window.native_window.request_redraw();
                }
                event::Event::RedrawRequested(_) if !self.destroyed && !self.minimized => {
                    if let Err(error) = self.render_context.begin_frame(&self.window.native_window)
                    {
                        error!("Failed to begin frame: {}", error);
                        std::process::exit(1);
                    }

                    self.render_context.draw();

                    if let Err(error) = self.render_context.end_frame() {
                        error!("Failed to end frame: {}", error);
                        std::process::exit(1);
                    }

                    if let Err(error) = self
                        .render_context
                        .submit(&self.window.native_window, &mut self.resized)
                    {
                        error!("Failed to submit: {}", error);
                        std::process::exit(1);
                    }
                }
                _ => (),
            }
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
