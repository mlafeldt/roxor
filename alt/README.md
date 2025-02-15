# Alternative implementations

roxor was originally written in C and later reimplemented in other languages for fun and profit.

Even though Go is now the main implementation, you can still use the other ones.

## C

```console
zig build-exe roxor.c -O ReleaseFast --library c
```

## Zig

```console
zig build --release=fast
```

## Rust

```console
cargo build --release
```

## Crystal

```console
crystal build --release roxor.cr
```

## Ruby

There's also a Ruby implementation, `roxor.rb`, which works as-is.
