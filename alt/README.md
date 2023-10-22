# Alternative implementations

roxor was originally written in C and later reimplemented in other languages for fun and profit.

Even though Go is now the main implementation, you can still use the other ones.

## C

To build roxor from `roxor.c`, simply run:

    $ make
    $ make install

CMake is supported as well:

    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make
    $ make install

## Ruby

There's also a Ruby implementation, `roxor.rb`, that works out of the box.

## Crystal

To compile the Crystal implementation:

    $ crystal build --release roxor.cr

## Rust

To compile the Rust implementation:

    $ cargo build --release

## Zig

To compile the Zig implementation:

    $ zig build-exe -O ReleaseFast roxor.zig
