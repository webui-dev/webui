/*
    WebUI Library 2.0.5
    
    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

fn main() {

    // Path to prebuilt WebUI static library
    println!("cargo:rustc-link-search=../../../build/Windows/MSVC/");

    // Path to WebUI include folder (Optional)
    println!("cargo:rustc-link-search=../../../include/");   
    
    // WebUI static library name
    println!("cargo:rustc-link-lib=webui-2-static-x64");
    println!("cargo:rustc-link-lib=user32");
}
