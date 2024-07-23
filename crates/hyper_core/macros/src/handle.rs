//
// Copyright (c) 2023-2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use proc_macro::TokenStream;
use syn::{Data, DeriveInput, Error, Fields, Type};

pub fn derive_handle(input: TokenStream) -> TokenStream {
    let ast = syn::parse_macro_input!(input as DeriveInput);

    let Data::Struct(data_struct) = &ast.data else {
        let token_span = match &ast.data {
            Data::Enum(data_enum) => data_enum.enum_token.span,
            Data::Union(data_union) => data_union.union_token.span,
            _ => unreachable!(),
        };

        return Error::new(
            token_span,
            "Invalid data type for Handle, expected `struct`",
        )
        .to_compile_error()
        .into();
    };

    let Fields::Unnamed(unnamed_fields) = &data_struct.fields else {
        let ident_span = ast.ident.span();

        return Error::new(
            ident_span,
            "Invalid struct type for Handle, expected `unnamed struct`",
        )
        .to_compile_error()
        .into();
    };

    let fields = &unnamed_fields.unnamed;

    let Some(field) = fields.first() else {
        return Error::new_spanned(
            fields,
            "Invalid amount of unnamed fields for Handle, expected only one field",
        )
        .to_compile_error()
        .into();
    };

    let Type::Path(path_ty) = &field.ty else {
        return Error::new_spanned(
            fields,
            "Invalid unnamed field type for Handle, expected `Path`",
        )
        .to_compile_error()
        .into();
    };

    let path = &path_ty.path;
    if !path.is_ident("u32") {
        return Error::new_spanned(
            fields,
            "Invalid unnamed field data type for Handle, expected `u32`",
        )
        .to_compile_error()
        .into();
    }

    let struct_name = ast.ident;
    TokenStream::from(quote::quote! {
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
    })
}
