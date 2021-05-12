use std::ffi::c_void;

#[repr(C)]
struct Allocator {
    alloc: extern "C" fn(*mut c_void, usize) -> *mut c_void,
    realloc: extern "C" fn(*mut c_void, *mut c_void, usize) -> *mut c_void,
    free: extern "C" fn(*mut c_void, *mut c_void),
    data: *mut c_void,
}

#[repr(C)]
struct RGBA128 {
    r: u32,
    g: u32,
    b: u32,
    a: u32,
}

#[repr(C)]
struct Color {
    allocator: *const Allocator,
    c_id: i32,
    color: *mut c_void,
    size: usize,
    rgba128: extern "C" fn(*mut RGBA128, *mut c_void),
}

#[repr(C)]
struct Image {
    allocator: *const Allocator,
    img: *mut c_void,
    size: usize,
    w: i32,
    h: i32,
    color_model: extern "C" fn(*mut Color, *const Color),
    at: extern "C" fn(*const Image, i32, i32, *mut Color),
    set: extern "C" fn(*mut Image, i32, i32, *const Color),
}

extern "C" {
    pub static im_std_allocator: *const Allocator;
}

extern "C" {
    pub fn im_load_defaults();
}

pub type Rfun = extern "C" fn(*mut c_void, *mut u8, usize) -> i32;
pub type Wfun = extern "C" fn(*mut c_void, *const u8, usize) -> i32;
