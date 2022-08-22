/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use backtrace::Backtrace;
use log::error;
use std::{panic, thread};

pub fn init() {
    panic::set_hook(Box::new(move |panic_info| {
        let backtrace = if cfg!(debug_assertions) {
            Backtrace::default()
        } else {
            Backtrace::from(vec![])
        };

        let current_thread = thread::current();
        let current_thread_name = current_thread.name().unwrap_or("<unnamed>");

        let message = match panic_info.payload().downcast_ref::<&'static str>() {
            Some(s) => *s,
            None => match panic_info.payload().downcast_ref::<String>() {
                Some(s) => &**s,
                None => "Box<Any>",
            },
        };

        match panic_info.location() {
            Some(location) => {
                error!(
                    "{} in '{}' thread at {}:{}\n{:?}",
                    message,
                    current_thread_name,
                    location.file(),
                    location.line(),
                    backtrace
                );
            }
            None => error!(
                "{} in '{}' thread \n{:?}",
                message, current_thread_name, backtrace
            ),
        }
    }));
}
