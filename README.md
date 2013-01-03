roxor
=====

Tool to crack XOR-encrypted passwords


Installation
------------

To build roxor from source, simply run:

    $ git clone git://github.com/mlafeldt/roxor.git
    $ cd roxor/
    $ make
    $ make install

CMake is supported as well:

    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make
    $ make install


Usage
-----

    usage: roxor <file> <crib>

Examples
--------

roxor was able to get the passwords for these PSX games:

*Colin McRae Rally*

    $ roxor FRONTEND.EXE openroads
    Found text at 0x51ff8 (XOR key 0x13)
      preview: openroads...shoeboxes...silkysmooth.backseat....mo

*Need For Speed 3*

    $ roxor NFS3-Benutzername.bin playtm
    Found text at 0x13f4c (XOR key 0x01)
      preview: playtm..xcav8...xcntry..mnbeam..gldfsh..mcityz..se

*TOCA - World Touring Cars*

    $ roxor SLES_025.72 THUMBS
    Found text at 0x3d534 (XOR key 0x0f)
      preview: THUMBS..REARFEAR....MADCAB..VANISHING...KERBKRAWL.


License
-------

roxor is licensed under the terms of the MIT License. See [LICENSE] file.


Contact
-------

* Web: <http://mlafeldt.github.com/roxor>
* Mail: <mathias.lafeldt@gmail.com>
* Twitter: [@mlafeldt](https://twitter.com/mlafeldt)


[LICENSE]: https://github.com/mlafeldt/roxor/blob/master/LICENSE
