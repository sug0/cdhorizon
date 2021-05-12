use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    // change directory to libcdhorizon
    let libcdhorizon_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap())
        .join("..")
        .join("libcdhorizon");
    env::set_current_dir(&libcdhorizon_dir).unwrap();

    // compile code...
    Command::new("./make")
        .arg("build")
        .spawn()
        .map(|mut ch| ch.wait().unwrap())
        .unwrap();

    // link; TODO: make this portable
    println!("cargo:rustc-link-search=native={}", libcdhorizon_dir.to_str().unwrap());
    println!("cargo:rustc-link-search=native={}", "/usr/local/lib");
    println!("cargo:rustc-link-search=native={}", "/usr/lib");
    println!("cargo:rustc-link-lib=static={}", "cdhorizon");
    println!("cargo:rustc-link-lib=static={}", "goimg");
    println!("cargo:rustc-link-lib=static={}", "png");
    println!("cargo:rustc-link-lib=static={}", "jpeg");
    println!("cargo:rustc-link-lib=static={}", "m-2.32");
    println!("cargo:rustc-link-lib=static={}", "mvec");
    println!("cargo:rustc-link-lib=static={}", "dl");
    println!("cargo:rustc-link-lib=static={}", "z");
}
