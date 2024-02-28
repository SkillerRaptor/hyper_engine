/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use proc_macro::TokenStream;
use syn::{Data, DeriveInput, Error, Fields, Type};

#[proc_macro_derive(Handle)]
pub fn my_derive(input: TokenStream) -> TokenStream {
    let ast = syn::parse_macro_input!(input as DeriveInput);

    let fields = match &ast.data {
        Data::Struct(data_struct) => match &data_struct.fields {
            Fields::Unnamed(unnamed) => &unnamed.unnamed,
            _ => {
                let ident_span = ast.ident.span();

                return Error::new(
                    ident_span,
                    "Invalid struct type for Handle, expected `unnamed struct`",
                )
                .to_compile_error()
                .into();
            }
        },
        data => {
            let data_type_span = match data {
                Data::Enum(data_enum) => data_enum.enum_token.span,
                Data::Union(data_union) => data_union.union_token.span,
                _ => unreachable!(),
            };

            return Error::new(
                data_type_span,
                "Invalid data type for Handle, expected `struct`",
            )
            .to_compile_error()
            .into();
        }
    };

    if fields.len() != 1 {
        return Error::new_spanned(
            fields,
            "Invalid amount of unnamed fields for Handle, expected only one field",
        )
        .to_compile_error()
        .into();
    }

    let field = fields.first().unwrap();
    match &field.ty {
        Type::Path(path) => {
            let path = &path.path;
            if !path.is_ident("u32") {
                return Error::new_spanned(
                    fields,
                    "Invalid unnamed field data type for Handle, expected `u32`",
                )
                .to_compile_error()
                .into();
            }
        }
        _ => {
            return Error::new_spanned(
                fields,
                "Invalid unnamed field type for Handle, expected `Path`",
            )
            .to_compile_error()
            .into()
        }
    };

    let struct_name = ast.ident;

    let output = quote::quote! {
        impl ::hyper_core::handle::Handle for #struct_name {
            fn from_id(id: ::hyper_core::handle::IdType) -> Self {
                Self(id << Self::ID_SHIFT)
            }

            fn value(&self) -> ::hyper_core::handle::ValueType {
                self.0
            }

            fn value_mut(&mut self) -> &mut ::hyper_core::handle::ValueType {
                &mut self.0
            }
        }
    };

    TokenStream::from(output)
}
