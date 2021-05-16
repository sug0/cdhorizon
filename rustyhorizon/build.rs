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

    // link
    println!("cargo:rustc-link-search=native={}", libcdhorizon_dir.to_str().unwrap());
    println!("cargo:rustc-link-search=native={}", "/usr/local/lib");
    println!("cargo:rustc-link-search=native={}", "/usr/lib");
    println!("cargo:rustc-link-lib={}", "static=cdhorizon");
    println!("cargo:rustc-link-lib={}", "static=goimg");
    println!("cargo:rustc-link-lib={}", "luajit-5.1");
    println!("cargo:rustc-link-lib={}", "jpeg");
    println!("cargo:rustc-link-lib={}", "png");
}
