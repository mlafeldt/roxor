#!/usr/bin/env ruby

module Roxor
  class Application
    def initialize(argv)
      @args = argv
    end

    def run
      abort "usage: roxor <file> <crib>" if @args.length < 2

      filename = @args.shift
      crib = bytes(@args.shift)
      ciphertext = bytes(File.read(filename))

      abort "error: crib too short" if crib.length < 2
      abort "error: ciphertext too short" if ciphertext.length < crib.length

      attack_cipher(ciphertext, crib) do |offset, key|
        puts "Found text at 0x%x (XOR key 0x%02x)" % [offset, key]
        puts "  preview: %s" %
          (ciphertext[offset, 50].map do |ch|
            ch ^= key
            printable?(ch) ? "%c" % ch : "."
          end.join)
      end
    end

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

    def bytes(str)
      str.bytes.to_a
    end

    def printable?(ch)
      ch >= 32 && ch <= 126
    end
  end
end

if $0 == __FILE__
  Roxor::Application.new(ARGV).run
end
