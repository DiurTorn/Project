extern crate wasm_bindgen;

use wasm_bindgen::prelude::*;

#[wasm_bindgen]
extern "C" {

    type HTMLDocument;
    type Element;

    static document: HTMLDocument;

    #[wasm_bindgen(method)]
    fn createElement(this: &HTMLDocument, tagName: &str) -> Element;

    #[wasm_bindgen(method, getter)]
    fn body(this: &HTMLDocument) -> Element;

    #[wasm_bindgen(method, js_name = appendChild)]
    fn append(this: &Element, item: Element);

    #[wasm_bindgen(method, js_name = src)]
    fn src(this: &Element, src: &str);

    #[wasm_bindgen(method, js_name = type)]
    fn typeE(this: &Element, typeEl: &str);

    #[wasm_bindgen(method, setter = innerHTML)]
    fn set_inner(this: &Element, html: &str);
}

#[wasm_bindgen]
pub fn read_img(ptr: *mut u8, len: usize, buf: &str) {
    let img = unsafe { Vec::from_raw_parts(ptr, len, len) };

    // let img = match image::load_from_memory(&img) {
    //     Ok(i) => i,
    //     Err(e) => {
    //         return;
    //    }
    // };
}

pub fn file_img() {
    let div = document.createElement("div");
    let nameF = document.createElement("input");
    let inf = document.createElement("p");
    let img = document.createElement("img");

    inf.set_inner("Img");
    div.append(inf);
    div.append(nameF);
    div.append(img);

    document.body().append(div);
}
