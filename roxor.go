package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
	"log"
)

// Does the printing for roxor
func printText(pos int, cyphertext []byte, key byte) {
	fmt.Printf("Found text at 0x%x (XOR key 0x%02x\n", pos, key)
	fmt.Printf("  preview: ")
	for _, val := range cyphertext[:50] {
		ch := val ^ key
		if strconv.IsPrint(rune(ch)) {
			fmt.Printf("%v", ch)
		} else {
			fmt.Printf(".")
		}
	}
	fmt.Println()
}

// the actual de-cipher
func attackCipher(ciphertext, crib []byte) {
	var key byte

	for i, val := range ciphertext {
		/* could we even fully match crib in the remaining text? */
		if len(ciphertext)-i < len(crib) {
			break
		}

		oldkey := val ^ crib[0]

		match := true

		cipherpos := ciphertext[i+1:]
		for j, crypted := range crib[1:] {
			key = cipherpos[j] ^ crypted
			if key != oldkey {
				match = false
			}
		}

		if match {
			printText(i, ciphertext[i:], key)
		}
	}
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

	attackCipher(ciphertext, crib)
}
