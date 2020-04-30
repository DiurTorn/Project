use diesel;
use diesel::pg::PgConnection;
use diesel::prelude::*;

use crate::schema::images;
use crate::schema::images::dsl::images as all_images;

#[derive(Queryable, Debug, Clone)]
pub struct Image {
    pub id: i32,
    pub nameimg: String,
    pub img: String,
}

#[derive(Insertable)]
#[table_name = "images"]
pub struct NewImage {
    pub nameimg: String,
    pub img: String,
}

impl Image {
    pub fn show(id: i32, conn: &PgConnection) -> Vec<Image> {
        all_images
            .find(id)
            .load::<Image>(conn)
            .expect("Error loading image")
    }

    pub fn all(conn: &PgConnection) -> Vec<Image> {
        all_images
            .order(images::id.desc())
            .load::<Image>(conn)
            .expect("Error loading the images")
    }

    pub fn update_by_id(id: i32, conn: &PgConnection, imag: NewImage) -> bool {
        use crate::schema::images::dsl::{img as i, nameimg as n};
        let NewImage { nameimg, img } = imag;

        diesel::update(all_images.find(id))
            .set((n.eq(nameimg), i.eq(img)))
            .get_result::<Image>(conn)
            .is_ok()
    }

    pub fn insert(imag: NewImage, conn: &PgConnection) -> bool {
        diesel::insert_into(images::table)
            .values(&imag)
            .execute(conn)
            .is_ok()
    }

    pub fn delete_by_id(id: i32, conn: &PgConnection) -> bool {
        if Image::show(id, conn).is_empty() {
            return false;
        };
        diesel::delete(all_images.find(id)).execute(conn).is_ok()
    }

    pub fn all_by_nameimg(imgname: String, conn: &PgConnection) -> Vec<Image> {
        all_images
            .filter(images::nameimg.eq(imgname))
            .load::<Image>(conn)
            .expect("Error loading images")
    }
}
