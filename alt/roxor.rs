use std::env;
use std::error::Error;
use std::fs::File;
use std::io::prelude::*;
use std::path::Path;
use std::process;

fn isprint(ch: u8) -> bool {
    ch >= 32 && ch <= 126
}

fn attack_cipher(ciphertext: Vec<u8>, crib: Vec<u8>) {
    for (i, x) in ciphertext.iter().enumerate() {
        let mut old_key = x ^ crib[0];
        let mut j = 1;
        while j < crib.len() && (i+j) < ciphertext.len() {
            let key = ciphertext[i+j] ^ crib[j];
            if key != old_key { break; }
            old_key = key;
            j += 1;
            if j == crib.len() {
                println!("Found text at 0x{:x} (XOR key 0x{:02x})", i, key);
                print!("  preview: ");
                for ch in &ciphertext[i..i+50] {
                    let raw = ch ^ key;
                    if isprint(raw) {
                        print!("{}", raw as char);
                    } else {
                        print!(".");
                    }
                }
                println!("");
            }
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 3 {
        println!("usage: roxor <file> <crib>");
        process::exit(1);
    }

    let path = Path::new(&args[1]);
    let crib: Vec<u8> = args[2].bytes().collect();

    if crib.len() < 2 {
        println!("error: crib too short");
        process::exit(1);
    }

    let mut file = match File::open(&path) {
        Err(why) => panic!("couldn't open {}: {}", path.display(), Error::description(&why)),
        Ok(file) => file,
    };

    let mut ciphertext: Vec<u8> = Vec::new();
    file.read_to_end(&mut ciphertext).unwrap();

    if ciphertext.len() < crib.len() {
        println!("error: ciphertext too short");
        process::exit(1);
    }

    attack_cipher(ciphertext, crib);
}
