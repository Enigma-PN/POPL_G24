use std::ptr;

use image::{Rgb, RgbImage, DynamicImage, GenericImageView};

trait Image {
    fn new_from_file(filename: &str) -> Self;
    fn new_rgb(w: u32, h: u32) -> Self;
    fn read(&mut self, filename: &str) -> bool;
    fn write(&self, filename: &str) -> bool;
    fn get_file_type(filename: &str) -> ImageType;
    fn grayscale_lum(&mut self) -> &mut Self;
    fn grayscale_avg(&mut self) -> &mut Self;
    fn color_mask(&mut self, r: f32, g: f32, b: f32) -> &mut Self;
    fn color_swap_rg(&mut self) -> &mut Self;
    fn rem_red(&mut self) -> &mut Self;
    fn encode_message(&mut self, message: &str) -> &mut Self;
    fn decode_message(&self, buffer: &mut [u8], message_length: &mut usize) -> &Self;
}

#[derive(Debug, PartialEq)]
enum ImageType {
    PNG,
    JPG,
    BMP,
    TGA,
    UNKNOWN,
}

const STEG_HEADER_SIZE: u32 = 32*8; // Assuming 32 bits for header

struct RustImage {
    image: RgbImage,
}

impl Image for RustImage {
    fn new_from_file(filename: &str) -> Self {
        let mut rust_image = RustImage {
            image: RgbImage::new(1, 1),
        };
        if rust_image.read(filename) {
            println!("Read {}", filename);
            let (w, h) = rust_image.image.dimensions();
            let size = w * h * 3; // Assuming 3 channels (RGB)
            println!("{} {} {}", w, h, size);
        } else {
            println!("Error reading file {}", filename);
        }
        rust_image
    }

    fn new_rgb(w: u32, h: u32) -> Self {
        RustImage {
            image: RgbImage::new(w, h),
        }
    }

    fn read(&mut self, filename: &str) -> bool {
        match image::open(filename) {
            Ok(image) => {
                let rgb_image = image.to_rgb8();
                self.image = RgbImage::from_vec(rgb_image.width(), rgb_image.height(), rgb_image.into_raw()).unwrap();
                true
            }
            Err(_) => false,
        }
    }

    fn write(&self, filename: &str) -> bool {
        match Self::get_file_type(filename) {
            ImageType::PNG => self.image.save(filename).is_ok(),
            _ => {
                println!("Unsupported image type.");
                false
            }
        }
    }

    fn get_file_type(filename: &str) -> ImageType {
        let ext = filename.split('.').last();
        match ext {
            Some("png") => ImageType::PNG,
            Some("jpg") => ImageType::JPG,
            Some("bmp") => ImageType::BMP,
            Some("tga") => ImageType::TGA,
            _ => ImageType::UNKNOWN,
        }
    }

    fn grayscale_lum(&mut self) -> &mut Self {
        for pixel in self.image.pixels_mut() {
            let gray = (0.2126 * pixel[0] as f32 + 0.7152 * pixel[1] as f32 + 0.0722 * pixel[2] as f32) as u8;
            *pixel = Rgb([gray, gray, gray]);
        }
        self
    }

    fn grayscale_avg(&mut self) -> &mut Self {
        for pixel in self.image.pixels_mut() {
            let gray = ((pixel[0] as u32 + pixel[1] as u32 + pixel[2] as u32) / 3) as u8;
            *pixel = Rgb([gray, gray, gray]);
        }
        self
    }

    fn color_mask(&mut self, r: f32, g: f32, b: f32) -> &mut Self {
        for pixel in self.image.pixels_mut() {
            pixel[0] = (pixel[0] as f32 * r) as u8;
            pixel[1] = (pixel[1] as f32 * g) as u8;
            pixel[2] = (pixel[2] as f32 * b) as u8;
        }
        self
    }

    fn color_swap_rg(&mut self) -> &mut Self {
        for pixel in self.image.pixels_mut() {
            let temp = pixel[0];
            pixel[0] = pixel[1];
            pixel[1] = temp;
        }
        self
    }
    
    fn rem_red(&mut self) -> &mut Self {
        for pixel in self.image.pixels_mut() {
            if pixel[0] > pixel[1] && pixel[0] > pixel[2] {
                let temp = pixel[0];
                pixel[0] = pixel[1];
                pixel[1] = temp;
            }
        }
        self
    }

    fn encode_message(&mut self, message: &str) -> &mut Self {
        let len = message.len() as u32 * 8;
        if len + 256 > self.image.dimensions().0 * self.image.dimensions().1 * 3 {
            println!(
                "[ERROR] This message is too large ({} bits / {} bits)",
                len + 256,
                self.image.dimensions().0 * self.image.dimensions().1 * 3
            );
            return self;
        }

        for i in 0..256 {
            self.image.pixels_mut().nth(i as usize).map(|pixel| {
                pixel[0] &= 0xFE;
                let shift_amount = (256 - 1 - i).min(31); // Ensure shift_amount is within the valid range
                pixel[0] |= (len >> shift_amount) as u8 & 1;
            });
        }
        

        for i in 0..len {
            self.image.pixels_mut().nth((i + 256) as usize).map(|pixel| {
                pixel[0] &= 0xFE;
                let shift_amount = (len - i % 8).min(7); // Ensure shift_amount is within the valid range
                pixel[0] |= (message.as_bytes()[i as usize / 8] >> shift_amount) as u8 & 1;
            });
        }
        

        self
    }

    fn decode_message(&self, buffer: &mut [u8], message_length: &mut usize) -> &Self {
        let mut len = 0;
        for i in 0..256 {
            self.image.pixels().nth(i as usize).map(|pixel| {
                len = (len << 1) | (pixel[0] as u32 & 1);
            });
        }
        *message_length = len as usize / 8;


        for i in 0..len {
            self.image.pixels().nth((i + 256) as usize).map(|pixel| {
                buffer[i as usize / 8] = (buffer[i as usize / 8] << 1) | (pixel[0] as u8 & 1);
            });
        }

        self
    }    
}

extern crate lsb_text_png_steganography;
use lsb_text_png_steganography::hide;
use lsb_text_png_steganography::reveal;


fn run () {
    let payload_path = "./message.txt";
    let carrier_path = "image.png";
    let output_carrier_path = "results/output.png";
   
   // hide
    let img = hide(payload_path, carrier_path);
    img.save(output_carrier_path).unwrap();

    // reveal
    let text = reveal(output_carrier_path);
    println!("{}", text)
}

fn main() {
    let mut rust_image = RustImage::new_from_file("image.png");

    // rust_image.grayscale_lum().write("results/grayscale_lum.png");
    // rust_image.grayscale_avg().write("results/grayscale_avg.png");
    // rust_image.color_mask(0.5, 1.0, 0.5).write("results/color_mask.png");
    // rust_image.color_swap_rg().write("results/color_swap_rg.png");
    // rust_image.rem_red().write("results/rem_red.png");

    run();
}

