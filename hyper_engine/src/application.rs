/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::{Window, WindowError};
use hyper_rendering::context::RenderContext;

use colored::Colorize;
use log::error;

pub enum ApplicationError {
    IoError(std::io::Error),
    LogError(log::SetLoggerError),
    WindowError(WindowError),
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
            ApplicationError::WindowError(error) => {
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

impl From<WindowError> for ApplicationError {
    fn from(error: WindowError) -> Self {
        ApplicationError::WindowError(error)
    }
}

pub struct Application {
    window: Window,
    render_context: RenderContext,

    resized: bool,
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
            window,
            render_context,

            resized: false,
        }
    }

    pub fn run(&mut self) {
        let mut fps: u32 = 0;
        let mut last_frame = std::time::Instant::now();
        let mut last_fps_frame = std::time::Instant::now();
        while !self.window.should_close() {
            let current_frame = std::time::Instant::now();
            let delta_time = current_frame.duration_since(last_frame).as_secs_f64();

            while current_frame.duration_since(last_fps_frame).as_secs_f64() >= 1.0 {
                self.window.set_title(
                    format!("HyperEngine (Delta Time: {}, FPS: {})", delta_time, fps).as_str(),
                );
                fps = 0;
                last_fps_frame = current_frame;
            }

            // NOTE: Update
            self.window.poll_events();
            self.window.handle_events(|event| match event {
                glfw::WindowEvent::FramebufferSize(_, _) => self.resized = true,
                _ => {}
            });

            // NOTE: Render
            self.render_context.begin_frame(&self.window);
            self.render_context.draw(&self.window);
            self.render_context.end_frame();
            self.render_context.submit(&self.window, &mut self.resized);

            fps += 1;
            last_frame = current_frame;
        }
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
