use actix_web::{web, App, HttpServer, HttpResponse, Responder};
use futures_util::{TryStreamExt, StreamExt};
use actix_multipart::Multipart;
use std::default::Default;
use serde::Serialize;

macro_rules! error {
    ($e:expr) => {{
        let payload: String = $e.into();
        let payload: Response = payload.into();
        return HttpResponse::BadRequest().json(payload);
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

#[derive(Default, Serialize)]
struct Response {
    error: Option<String>,
}

impl From<String> for Response {
    fn from(s: String) -> Self {
        Self { error: Some(s) }
    }
}

async fn handler(mut payload: Multipart) -> impl Responder {
    let mut has_script = false;
    let mut has_image = false;

    while let Ok(Some(mut field)) = payload.try_next().await {
        if has_script && has_image {
            error!("Too many fields in multipart form.");
        }

        let disp = some!(field.content_disposition(), "Invalid content disposition.");
        let name = some!(disp.get_name(), "No \"name\" found in multipart.");

        match name {
            "script" if has_script => error!("Duplicate \"script\" field found."),
            "script" => has_script = true,
            "image" if has_image => error!("Duplicate \"image\" field found."),
            "image" => has_image = true,
            _ => error!("Invalid multipart field name."),
        }

        while let Some(chunk) = field.next().await {
            let _chunk = ok!(chunk, "Failed decoding multipart data.");
        }
    }

    HttpResponse::Ok().json(Response::default())
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
