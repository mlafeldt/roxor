use std::env;
use std::fs::File;
use std::io::prelude::*;
use std::io::stderr;
use std::path::Path;
use std::process;

const PREVIEW_LEN: usize = 50;

#[derive(Debug, PartialEq)]
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
    let crib = args[1].as_bytes();

    if crib.len() < 2 {
        writeln!(stderr(), "error: crib too short").unwrap();
        process::exit(1);
    }

    let mut file = match File::open(&path) {
        Err(why) => panic!("couldn't open {}: {}", path.display(), why),
        Ok(file) => file,
    };

    let mut ciphertext: Vec<u8> = Vec::new();
    file.read_to_end(&mut ciphertext).unwrap();

    if ciphertext.len() < crib.len() {
        writeln!(stderr(), "error: ciphertext too short").unwrap();
        process::exit(1);
    }

    for m in attack_cipher(&ciphertext[..], crib) {
        println!("Found text at 0x{:x} (XOR key 0x{:02x})", m.offset, m.key);
        println!("  preview: {}", m.preview);
    }
}

fn attack_cipher(ciphertext: &[u8], crib: &[u8]) -> Vec<Match> {
    let mut matches: Vec<Match> = Vec::new();
    for (i, x) in ciphertext.iter().enumerate() {
        let mut old_key = x ^ crib[0];
        let mut j = 1;
        while j < crib.len() && (i + j) < ciphertext.len() {
            let key = ciphertext[i + j] ^ crib[j];
            if key != old_key {
                break;
            }
            old_key = key;
            j += 1;
            if j == crib.len() {
                let preview: String = ciphertext[i..]
                                          .iter()
                                          .take(PREVIEW_LEN)
                                          .map(|x| x ^ key)
                                          .map(|x| {
                                              if x >= 32 && x <= 126 {
                                                  x as char
                                              } else {
                                                  '.'
                                              }
                                          })
                                          .collect();
                matches.push(Match {
                    offset: i,
                    key: key,
                    preview: preview,
                });
            }
        }
    }
    return matches;
}

#[cfg(test)]
mod tests {
    use super::{Match, attack_cipher};

    struct Test {
        ciphertext: &'static [u8],
        crib: &'static [u8],
        matches: Vec<Match>,
    }

    #[test]
    fn test_attack_cipher() {
        let tests = vec![
            Test {
                ciphertext: &[],
                crib: &[],
                matches: vec![],
            },
            Test {
                ciphertext: b"haystack",
                crib: b"needle",
                matches: vec![],
            },
            Test {
                ciphertext: b"needle in haystack",
                crib: b"needle",
                matches: vec![
                    Match { offset: 0, key: 0, preview: "needle in haystack".to_string() },
                ],
            },
            Test {
                ciphertext: b"a needle, another needle",
                crib: b"needle",
                matches: vec![
                    Match { offset: 2, key: 0, preview: "needle, another needle".to_string() },
                    Match { offset: 18, key: 0, preview: "needle".to_string() },
                ],
            },
            Test {
                ciphertext: b"\x23\x62\x2c\x27\x27\x26\x2e\x27\x6e\x62\x23\x2c\x2d\x36\x2a\x27\x30\x62\x2c\x27\x27\x26\x2e\x27",
                crib: b"needle",
                matches: vec![
                    Match { offset: 2, key: 0x42, preview: "needle, another needle".to_string() },
                    Match { offset: 18, key: 0x42, preview: "needle".to_string() },
                ],
            },
        ];

        for t in tests.iter() {
            assert_eq!(t.matches, attack_cipher(t.ciphertext, t.crib));
        }
    }
}
