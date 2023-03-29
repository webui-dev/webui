

fn main() {

    // Path to prebuilt WebUI static library
    println!("cargo:rustc-link-search=../../../build/Windows/MSVC/");

    // Path to WebUI include folder (Optional)
    println!("cargo:rustc-link-search=../../../include/");   
    
    // WebUI static library name
    println!("cargo:rustc-link-lib=webui-2-static-x64");
    println!("cargo:rustc-link-lib=user32");
}
