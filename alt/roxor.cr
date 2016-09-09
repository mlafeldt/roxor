module Roxor
  class Application
    def initialize(argv : Array(String))
      @args = argv
    end

    def run
      abort "usage: roxor <file> <crib>" if @args.size < 2

      filename = @args.shift
      crib = bytes(@args.shift)
      ciphertext = bytes(File.read(filename))

      abort "error: crib too short" if crib.size < 2
      abort "error: ciphertext too short" if ciphertext.size < crib.size

      attack_cipher(ciphertext, crib) do |offset, key|
        puts "Found text at 0x%x (XOR key 0x%02x)" % [offset, key]
        puts "  preview: %s" %
          ciphertext[offset, 50].map { |x|
            x = (x ^ key).unsafe_chr
            x.control? ? "." : x
          }.join
      end
    end

    def attack_cipher(ciphertext, crib, &block)
      offset = 0
      key = 0
      while offset < ciphertext.size
        oldkey = ciphertext[offset] ^ crib[0]
        i = 1

        while i < crib.size &&
              (offset + i) < ciphertext.size &&
              (key = ciphertext[offset + i] ^ crib[i]) == oldkey
          oldkey = key
          i += 1
        end

        if i == crib.size
          yield offset, key
        end

        offset += 1
      end
    end

    def bytes(str)
      str.bytes.to_a
    end
  end
end

Roxor::Application.new(ARGV).run
