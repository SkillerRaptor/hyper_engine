/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

pub trait Ignore: Sized {
    fn ignore(self) {}
}

impl<T, E> Ignore for Result<T, E> {}
