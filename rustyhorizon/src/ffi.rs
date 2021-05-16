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

#[repr(C)]
struct HorizonErrorCtx {
    data: *mut c_void,
    func: extern "C" fn(*mut c_void, *const u8),
}

#[repr(C)]
struct HorizonScript {
    lua: *mut c_void,
}

#[repr(C)]
union HorizonParamValue {
    int: i32,
    double: f64,
    string: *const u8,
}

#[repr(C)]
struct HorizonParam {
    kind: i32,
    key: *const u8,
    value: HorizonParamValue,
}

#[repr(C)]
struct HorizonParams {
    len: usize,
    list: HorizonParam,
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
