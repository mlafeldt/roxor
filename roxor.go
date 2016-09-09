package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"strconv"
)

const PreviewSize = 50

type Match struct {
	offset int
	key    byte
}

func attackCipher(ciphertext, crib []byte, f func(m Match)) []Match {
	var matches []Match
	var key byte = 0

	for i := 0; i < len(ciphertext); i++ {
		old_key := ciphertext[i] ^ crib[0]
		j := 1

		for (j < len(crib)) && ((i + j) < len(ciphertext)) {
			key = ciphertext[i+j] ^ crib[j]
			if key != old_key {
				break
			}
			old_key = key
			j++
		}

		if j == len(crib) {
			matches = append(matches, Match{i, key})

			if f != nil {
				f(Match{i, key})
			}
		}
	}

	return matches
}

func main() {
	log.SetFlags(0)

	args := os.Args[1:]
	if len(args) < 2 {
		log.Fatal("usage: roxor <file> <crib>")
	}

	filename := args[0]
	crib := []byte(args[1])

	if len(crib) < 2 {
		log.Fatal("error: crib too short")
	}

	ciphertext, err := ioutil.ReadFile(filename)
	if err != nil {
		log.Fatal(err)
	}

	if len(ciphertext) < len(crib) {
		log.Fatal("error: ciphertext too short")
	}

	attackCipher(ciphertext, crib, func(m Match) {
		fmt.Printf("Found text at 0x%x (XOR key 0x%02x)\n", m.offset, m.key)
		fmt.Printf("  preview: ")

		for i, ch := range ciphertext[m.offset:] {
			if i >= PreviewSize {
				break
			}
			ch ^= m.key
			if !strconv.IsPrint(rune(ch)) {
				ch = '.'
			}
			fmt.Printf("%c", ch)
		}

		fmt.Println()
	})
}
