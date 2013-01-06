#!/usr/bin/env ruby
#
# Ruby implementation of roxor.c
#
# Copyright (C) 2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
#
# This file is part of roxor.
#
# roxor is licensed under the terms of the MIT License. See LICENSE file.
#

# XXX Lots of "C" code ahead. There's probably a better way to do this in Ruby.
def attack_cipher(ciphertext, crib, &block)
  offset = 0
  while offset < ciphertext.length
    oldkey = ciphertext[offset] ^ crib[0]
    i = 1

    while i < crib.length &&
          (offset + i) < ciphertext.length &&
          (key = ciphertext[offset + i] ^ crib[i]) == oldkey
      oldkey = key
      i += 1
    end

    if i == crib.length
      yield offset, key if block_given?
    end

    offset += 1
  end
end

def isprint(ch)
    ch >= 32 && ch <= 126
end


abort "usage: roxor <file> <crib>" if ARGV.length < 2

filename = ARGV.shift
crib = ARGV.shift.bytes.to_a
ciphertext = File.read(filename).bytes.to_a

abort "error: crib too short" if crib.length < 2
abort "error: ciphertext too short" if ciphertext.length < crib.length

attack_cipher(ciphertext, crib) do |offset, key|
  puts "Found text at 0x%x (XOR key 0x%02x)" % [offset, key]
  puts "  preview: %s" %
    (ciphertext[offset, 50].map do |ch|
      ch ^= key
      isprint(ch) ? "%c" % ch : "."
    end.join)
end
