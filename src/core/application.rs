/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use colored::Colorize;
use core::panic;
use log::{error, info};
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
    window: Window,
    render_context: RenderContext,
}

impl Application {
    pub fn new() -> Self {
        if let Err(error) = Self::setup_logger() {
            error!("Failed to create logger: {}", error);
            panic!();
        }

        let window = match Window::new("HyperEngine", 1280, 720) {
            Ok(window) => window,
            Err(error) => {
                error!("Failed to create window: {}", error);
                panic!();
            }
        };

        let render_context = match RenderContext::new(&window) {
            Ok(window) => window,
            Err(error) => {
                error!("Failed to create render context: {}", error);
                panic!();
            }
        };

        info!("Successfully created application");
        Self {
            window,
            render_context,
        }
    }

    pub fn run(&mut self) {
        self.window.event_loop.run_return(|event, _, control_flow| {
            *control_flow = event_loop::ControlFlow::Poll;

            match event {
                event::Event::WindowEvent {
                    event: event::WindowEvent::CloseRequested,
                    window_id,
                } => {
                    if window_id == self.window.native_window.id() {
                        *control_flow = event_loop::ControlFlow::Exit;
                    }
                }
                event::Event::MainEventsCleared => {
                    // TODO: Main Loop
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

        info!("Successfully created logger");
        Ok(())
    }
}
