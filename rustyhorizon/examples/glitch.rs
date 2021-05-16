use std::fs;
use std::collections::HashMap;

fn main() {
    let params = HashMap::new();
    let script = br#"
        local width  = horizon.width - 1
        local height = horizon.height - 1

        for y=0,height do
            for x=0,width do
                local output = horizon.getpixel(x, y)

                output[1] = bit.bxor(output[1], 255)
                output[2] = bit.bxor(output[2], 255)
                output[3] = bit.bxor(output[3], 255)

                horizon.setpixel(x, y, output)
            end
        end
    "#;

    let image = std::env::args().next().unwrap();
    let image = fs::read(image).unwrap();
    let image = rustyhorizon::run_script(
        script,
        &image,
        params,
    ).unwrap();

    fs::write("output.png", image).unwrap();
}
