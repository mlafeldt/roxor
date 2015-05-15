use std::env;
use std::error::Error;
use std::fs::File;
use std::io::prelude::*;
use std::io::stderr;
use std::path::Path;
use std::process;

struct Match {
    offset: usize,
    key: u8,
    preview: String,
}

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    if args.len() < 2 {
        writeln!(stderr(), "usage: roxor <file> <crib>").unwrap();
        process::exit(1);
    }

    let path = Path::new(&args[0]);
    let crib: Vec<u8> = args[1].bytes().collect();

    if crib.len() < 2 {
        writeln!(stderr(), "error: crib too short").unwrap();
        process::exit(1);
    }

    let mut file = match File::open(&path) {
        Err(why) => panic!("couldn't open {}: {}", path.display(), Error::description(&why)),
        Ok(file) => file,
    };

    let mut ciphertext: Vec<u8> = Vec::new();
    file.read_to_end(&mut ciphertext).unwrap();

    if ciphertext.len() < crib.len() {
        writeln!(stderr(), "error: ciphertext too short").unwrap();
        process::exit(1);
    }

    match attack_cipher(ciphertext, crib) {
        Some(m) => {
            println!("Found text at 0x{:x} (XOR key 0x{:02x})", m.offset, m.key);
            println!("  preview: {}", m.preview);
        },
        None => {},
    };
}

fn attack_cipher(ciphertext: Vec<u8>, crib: Vec<u8>) -> Option<Match> {
    for (i, x) in ciphertext.iter().enumerate() {
        let mut old_key = x ^ crib[0];
        let mut j = 1;
        while j < crib.len() && (i+j) < ciphertext.len() {
            let key = ciphertext[i+j] ^ crib[j];
            if key != old_key { break; }
            old_key = key;
            j += 1;
            if j == crib.len() {
                let preview: String = ciphertext[i..i+50]
                    .iter()
                    .map(|x| x ^ key)
                    .map(|x| if x >= 32 && x <= 126 {x as char} else {'.'})
                    .collect();
                return Some(Match { offset: i, key: key, preview: preview })
            }
        }
    }
    return None
}
