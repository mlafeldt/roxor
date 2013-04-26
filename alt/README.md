Alternative Implementations
===========================

roxor was originally written in C and later reimplemented in Ruby and Go.

Even though Go is now the main implementation, you can still use the other ones.

C
-

To build roxor from `roxor.c`, simply run:

    $ make
    $ make install

CMake is supported as well:

    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make
    $ make install

Ruby
----

There's also a Ruby implementation, `roxor.rb`, that works out of the box.
