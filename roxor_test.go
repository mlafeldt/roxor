package main

import "testing"

var NoMatch = []Match{}
var Needle = []byte("needle")

var attackCipherTests = []struct {
	ciphertext []byte
	crib       []byte
	matches    []Match
}{
	{[]byte{}, []byte{}, NoMatch},
	{[]byte("haystack"), Needle, NoMatch},
	{[]byte("needle in haystack"), Needle, []Match{{0, 0}}},
	{[]byte("a needle, another needle"), Needle, []Match{{2, 0}, {18, 0}}},
	{[]byte{0x23, 0x62, 0x2c, 0x27, 0x27, 0x26, 0x2e, 0x27,
		0x6e, 0x62, 0x23, 0x2c, 0x2d, 0x36, 0x2a, 0x27,
		0x30, 0x62, 0x2c, 0x27, 0x27, 0x26, 0x2e, 0x27},
		Needle, []Match{{2, 0x42}, {18, 0x42}}},
}

func TestAttackCipher(t *testing.T) {
	for _, test := range attackCipherTests {
		got := attackCipher(test.ciphertext, test.crib, nil)
		want := test.matches

		if len(got) != len(want) {
			t.Errorf("expected length %d, got %d", len(want), len(got))
		} else {
			for i, m := range got {
				if m != want[i] {
					t.Errorf("expected match #%d to be %v, got %v", i, want[i], m)
				}
			}
		}
	}
}
