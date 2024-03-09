/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

mod handle;

use proc_macro::TokenStream;

#[proc_macro_derive(Handle)]
pub fn derive_handle(input: TokenStream) -> TokenStream {
    handle::derive_handle(input)
}
