mod ffi;

use std::ffi::CStr;
use std::ffi::c_void;
use std::sync::Once;
use std::collections::HashMap;

#[cfg(feature = "serde_derive")]
use serde::{Serialize, Deserialize};

#[derive(Debug)]
pub enum HorizonError {
    InvalidScript(String),
    DecodeErr,
    EncodeErr,
}

#[derive(Debug)]
#[cfg_attr(feature = "serde_derive", derive(Serialize, Deserialize))]
pub enum Param {
    #[serde(rename = "string")]
    String(String),
    #[serde(rename = "double")]
    Double(f64),
    #[serde(rename = "int")]
    Int(i32),
}

struct SliceReader {
    data: *const u8,
    len: usize,
    ptr: usize,
}

struct RunCtx {
    script: ffi::HorizonScript,
    src: ffi::Image,
    dst: ffi::Image,
}

static INIT: Once = Once::new();

pub fn run_script(script: &[u8], image: &[u8], params: HashMap<String, Param>) -> Result<Vec<u8>, HorizonError> {
    unsafe { run_script_unsafe(script, image, params) }
}

fn get_cstr(arena: &mut Vec<String>, mut s: String) -> *const u8 {
    // some ptr gymnastics to drop things correctly
    // and be compatible with c strings
    s.push_str("\0");
    arena.push(s);
    arena[arena.len()-1].as_ptr()
}

#[inline]
unsafe fn run_script_unsafe(script: &[u8], image: &[u8], params: HashMap<String, Param>) -> Result<Vec<u8>, HorizonError> {
    // init libgoimg image formats
    INIT.call_once(|| ffi::im_load_defaults());

    // data
    let mut ctx = RunCtx {
        script: ffi::HorizonScript { lua: std::ptr::null_mut() },
        src: ffi::Image {
            allocator: ffi::im_std_allocator,
            img: std::ptr::null_mut(),
            size: 0,
            w: 0,
            h: 0,
            color_model: None,
            at: None,
            set: None,
        },
        dst: ffi::Image {
            allocator: ffi::im_std_allocator,
            img: std::ptr::null_mut(),
            size: 0,
            w: 0,
            h: 0,
            color_model: None,
            at: None,
            set: None,
        },
    };

    // convert params to a suitable format
    let mut list = Vec::with_capacity(params.len());
    let mut str_arena = Vec::with_capacity(params.len());

    for (key, value) in params {
        let key = get_cstr(&mut str_arena, key);
        match value {
            Param::String(string) => {
                let string = get_cstr(&mut str_arena, string);
                list.push(ffi::HorizonParam {
                    key,
                    kind: ffi::HorizonParamKind::STRING,
                    value: ffi::HorizonParamValue { string },
                });
            },
            Param::Double(x) => list.push(ffi::HorizonParam {
                key,
                kind: ffi::HorizonParamKind::DOUBLE,
                value: ffi::HorizonParamValue { double: x },
            }),
            Param::Int(x) => list.push(ffi::HorizonParam {
                key,
                kind: ffi::HorizonParamKind::INT,
                value: ffi::HorizonParamValue { int: x },
            }),
        }
    }

    let params = ffi::HorizonParams { len: list.len(), list: list.as_ptr() };

    // compile script
    let mut reader = SliceReader {
        data: script.as_ptr(),
        len: script.len(),
        ptr: 0,
    };
    let mut error_ctx_data = String::new();
    let mut error_ctx = ffi::HorizonErrorCtx {
        data: error_ctx_data.as_mut_ptr() as *mut _,
        func: lua_error_to_str,
    };

    let ok = ffi::horizon_ScriptCompileCtxParams(
        &params as *const _,
        &mut error_ctx as *mut _,
        &mut ctx.script as *mut _,
        &mut reader as *mut _ as *mut _,
        read_slice,
    );
    if ok != 0 {
        return Err(HorizonError::InvalidScript(error_ctx_data));
    }

    // decode image
    let mut reader = SliceReader {
        data: image.as_ptr(),
        len: image.len(),
        ptr: 0,
    };

    let fmt = ffi::im_decode(
        &mut ctx.src as *mut _,
        read_slice,
        &mut reader as *mut _ as *mut _,
    );
    if fmt.is_null() {
        return Err(HorizonError::DecodeErr);
    }

    // glitch image
    ffi::im_initimg_nrgba(
        &mut ctx.dst as *mut _,
        ctx.src.w,
        ctx.src.h,
        ffi::im_std_allocator,
    );
    error_ctx_data.clear();

    let ok = ffi::horizon_GlitchCtx(
        &mut error_ctx as *mut _,
        &mut ctx.dst as *mut _,
        &ctx.src as *const _,
        &mut ctx.script as *mut _,
    );
    if ok != 0 {
        return Err(HorizonError::InvalidScript(error_ctx_data));
    }

    // encode image
    let mut dest = Vec::new();
    let fmt = "PNG\0".as_ptr();

    let ok = ffi::im_encode(
        &ctx.dst as *const _,
        fmt,
        write_vec,
        &mut dest as *mut _ as *mut _,
    );
    if ok < 0 {
        return Err(HorizonError::EncodeErr);
    }

    Ok(dest)
}

extern "C" fn read_slice(slice: *mut c_void, buf: *mut u8, bufsiz: usize) -> i32 {
    unsafe {
        let slice = &mut *(slice as *mut SliceReader);

        if slice.ptr == slice.len {
            return 0;
        }

        let read = slice.ptr + bufsiz;
        let read = if read > slice.len { slice.len - slice.ptr } else { bufsiz };

        buf.copy_from(slice.data.add(slice.ptr), read);
        slice.ptr += read;

        read as i32
    }
}

extern "C" fn write_vec(vec: *mut c_void, buf: *const u8, bufsiz: usize) -> i32 {
    unsafe {
        let vec = &mut *(vec as *mut Vec<u8>);
        let slice = std::slice::from_raw_parts(buf, bufsiz);
        vec.extend_from_slice(slice);
        bufsiz as i32
    }
}

extern "C" fn lua_error_to_str(s: *mut c_void, msg: *const u8) {
    unsafe {
        let s = &mut *(s as *mut String);
        let msg = CStr::from_ptr(msg as *const i8).to_string_lossy();
        s.push_str(&msg);
    }
}

impl std::ops::Drop for RunCtx {
    fn drop(&mut self) {
        unsafe {
            ffi::horizon_ScriptClose(&mut self.script as *mut _);
            ffi::im_xfree(ffi::im_std_allocator, self.src.img);
            ffi::im_xfree(ffi::im_std_allocator, self.dst.img);
        }
    }
}
