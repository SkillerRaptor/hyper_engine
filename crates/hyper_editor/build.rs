/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{env, fs, io, path::Path};

fn main() {
    let output_dir = env::var("OUT_DIR").unwrap() + "/../../..";

    if false {
        println!("cargo:rerun-if-changed=../../assets");
        copy_dir_all("../../assets", format!("{}/assets", output_dir)).unwrap();
    }

    if cfg!(target_os = "windows") {
        fs::copy(
            "../../dxcompiler.dll",
            format!("{}/dxcompiler.dll", output_dir),
        )
        .unwrap();
        fs::copy("../../dxil.dll", format!("{}/dxil.dll", output_dir)).unwrap();
    } else {
        fs::copy(
            "../../libdxcompiler.so",
            format!("{}/libdxcompiler.so", output_dir),
        )
        .unwrap();
        fs::copy("../../libdxil.so", format!("{}/libdxil.so", output_dir)).unwrap();
    }
}

fn copy_dir_all(source: impl AsRef<Path>, destination: impl AsRef<Path>) -> io::Result<()> {
    fs::create_dir_all(&destination)?;

    for entry in fs::read_dir(source)? {
        let entry = entry?;
        let ty = entry.file_type()?;
        if ty.is_dir() {
            copy_dir_all(entry.path(), destination.as_ref().join(entry.file_name()))?;
        } else {
            fs::copy(entry.path(), destination.as_ref().join(entry.file_name()))?;
        }
    }
    Ok(())
}
