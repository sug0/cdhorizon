use actix_web::{
    web,
    App,
    Responder,
    HttpServer,
    HttpResponse,
    rt::time::timeout,
    error::BlockingError,
};
use rustyhorizon::{run_script, Param, HorizonError};
use futures_util::{TryStreamExt, StreamExt};
use actix_multipart::Multipart;
use enum_map::{Enum, EnumMap};
use std::collections::HashMap;
use std::default::Default;
use std::time::Duration;

macro_rules! error {
    ($e:expr) => {{
        let payload: Vec<u8> = $e.as_bytes().into();
        return HttpResponse::BadRequest().body(payload);
    }}
}

macro_rules! some {
    ($f:expr, $e:expr) => {
        match $f {
            Some(x) => x,
            None => error!($e),
        }
    }
}

macro_rules! ok {
    ($f:expr, $e:expr) => {
        match $f {
            Ok(x) => x,
            Err(_) => error!($e),
        }
    }
}

#[derive(Enum)]
enum FieldType {
    Script,
    Image,
    Params,
}

async fn handler(mut payload: Multipart) -> impl Responder {
    let mut has_script = false;
    let mut has_image = false;
    let mut has_params = false;

    let mut map = EnumMap::default();

    while let Ok(Some(mut field)) = payload.try_next().await {
        if has_script && has_image && has_params {
            error!("Too many fields in multipart form.");
        }

        let disp = some!(field.content_disposition(), "Invalid content disposition.");
        let name = some!(disp.get_name(), "No \"name\" found in multipart.");

        let field_type = match name {
            "params" if has_params => error!("Duplicate \"params\" field found."),
            "params" => { has_params = true; FieldType::Params },
            "script" if has_script => error!("Duplicate \"script\" field found."),
            "script" => { has_script = true; FieldType::Script },
            "image" if has_image => error!("Duplicate \"image\" field found."),
            "image" => { has_image = true; FieldType::Image },
            _ => error!("Invalid multipart field name."),
        };

        let mut data = Vec::new();
        while let Some(chunk) = field.next().await {
            let chunk = ok!(chunk, "Failed decoding multipart data.");
            data.extend_from_slice(&chunk);
        }

        map[field_type] = Some(data);
    }

    let (image, script, params) = {
        (map[FieldType::Image].take(), map[FieldType::Script].take(), map[FieldType::Params].take())
    };

    let image = some!(image, "Image expected.");
    let script = some!(script, "Lua script expected.");
    let params: HashMap<String, Param> = if let Some(params) = &params {
        ok!(serde_json::from_slice(&params[..]), "Invalid \"params\".")
    } else {
        HashMap::new()
    };

    const MAX_DUR: Duration = Duration::from_secs(3 * 60);

    let future = web::block(move || run_script(&script[..], &image[..], params));
    let result = ok!(timeout(MAX_DUR, future).await, "Image operation took too long.");

    match result {
        Ok(image) => HttpResponse::Ok().body(image),
        Err(BlockingError::Canceled) => error!("Image operation was canceled."),
        Err(BlockingError::Error(HorizonError::DecodeErr)) => error!("Image decoding failed."),
        Err(BlockingError::Error(HorizonError::EncodeErr)) => error!("Image encoding failed."),
        Err(BlockingError::Error(HorizonError::InvalidScript(reason))) => error!(reason),
    }
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    HttpServer::new(|| {
        App::new()
            .route("/", web::post().to(handler))
    })
    .bind(("127.0.0.1", 1234))?
    .run()
    .await
}
