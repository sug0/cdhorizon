use std::ffi::c_void;

#[repr(C)]
pub struct Allocator {
    pub alloc: extern "C" fn(*mut c_void, usize) -> *mut c_void,
    pub realloc: extern "C" fn(*mut c_void, *mut c_void, usize) -> *mut c_void,
    pub free: extern "C" fn(*mut c_void, *mut c_void),
    pub data: *mut c_void,
}

#[repr(C)]
pub struct RGBA128 {
    pub r: u32,
    pub g: u32,
    pub b: u32,
    pub a: u32,
}

#[repr(C)]
pub struct Color {
    pub allocator: *const Allocator,
    pub c_id: i32,
    pub color: *mut c_void,
    pub size: usize,
    pub rgba128: extern "C" fn(*mut RGBA128, *mut c_void),
}

#[repr(C)]
pub struct Image {
    pub allocator: *const Allocator,
    pub img: *mut c_void,
    pub size: usize,
    pub w: i32,
    pub h: i32,
    pub color_model: extern "C" fn(*mut Color, *const Color),
    pub at: extern "C" fn(*const Image, i32, i32, *mut Color),
    pub set: extern "C" fn(*mut Image, i32, i32, *const Color),
}

#[repr(C)]
pub struct HorizonErrorCtx {
    pub data: *mut c_void,
    pub func: extern "C" fn(*mut c_void, *const u8),
}

#[repr(C)]
pub struct HorizonScript {
    pub lua: *mut c_void,
}

#[repr(C)]
pub union HorizonParamValue {
    pub int: i32,
    pub double: f64,
    pub string: *const u8,
}

#[repr(C)]
pub struct HorizonParam {
    pub kind: i32,
    pub key: *const u8,
    pub value: HorizonParamValue,
}

#[repr(C)]
pub struct HorizonParams {
    pub len: usize,
    pub list: *const HorizonParam,
}

extern "C" {
    pub static im_std_allocator: *const Allocator;
}

extern "C" {
    pub fn im_load_defaults();
    pub fn horizon_ScriptClose(script: *mut HorizonScript);
    pub fn horizon_ScriptCompileCtxParams(
        params: *const HorizonParams,
        ctx: *mut HorizonErrorCtx,
        script: *mut HorizonScript,
        src: *mut c_void,
        rfun: Rfun,
    ) -> i32;
    pub fn horizon_GlitchCtx(
        ctx: *mut HorizonErrorCtx,
        dst: *mut Image,
        src: *const Image,
        script: *mut HorizonScript,
    ) -> i32;
}

pub type Rfun = extern "C" fn(*mut c_void, *mut u8, usize) -> i32;
pub type Wfun = extern "C" fn(*mut c_void, *const u8, usize) -> i32;
