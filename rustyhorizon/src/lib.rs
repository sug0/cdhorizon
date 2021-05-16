mod ffi;

use std::sync::Once;
use std::ffi::c_void;
use std::collections::HashMap;

pub enum Param {
    String(String),
    Double(f64),
    Int(i32),
}

#[repr(C)]
struct SliceReader {
    data: *const u8,
    len: usize,
    ptr: usize,
}

static INIT: Once = Once::new();

pub fn run_script(script: &[u8], image: &[u8], params: HashMap<String, Param>) -> Vec<u8> {
    unsafe { run_script_unsafe(script, image, params) }
}

#[inline]
unsafe fn run_script_unsafe(script: &[u8], image: &[u8], params: HashMap<String, Param>) -> Vec<u8> {
    // init libgoimg image formats
    INIT.call_once(|| ffi::im_load_defaults());

    // convert params to a suitable format
    let mut list = Vec::with_capacity(params.len());
    for (key, value) in params {
        let key = key.as_ptr();
        match value {
            Param::String(s) => list.push(ffi::HorizonParam {
                key,
                kind: ffi::HorizonParamKind::STRING,
                value: ffi::HorizonParamValue { string: s.as_ptr() },
            }),
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

    // script reader
    let mut reader = SliceReader {
        data: script.as_ptr(),
        len: script.len(),
        ptr: 0,
    };

    unimplemented!()
}

unsafe extern "C" fn read_slice(slice: *mut c_void, buf: *mut u8, bufsiz: usize) -> i32 {
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

unsafe extern "C" fn write_vec(vec: *mut c_void, buf: *const u8, bufsiz: usize) -> i32 {
    let vec = &mut *(vec as *mut Vec<u8>);
    let slice = std::slice::from_raw_parts(buf, bufsiz);
    vec.extend_from_slice(slice);
    bufsiz as i32
}
