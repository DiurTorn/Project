#![feature(proc_macro)]
extern crate wasm_bindgen;

use wasm_bindgen::prelude::*;

mod img_new;

#[wasm_bindgen]
extern "C" {
    fn alert(s: &str);
}

#[wasm_bindgen]
pub fn inBD(mImg: &str, imgB: &str) {}

#[wasm_bindgen]
pub fn outBd() {
    alert("Нажата кнопка outBD");
}

#[wasm_bindgen]
pub fn close() {
    alert("Нажата кнопка Close");
}
