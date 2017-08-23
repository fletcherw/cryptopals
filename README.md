# cryptopals

This repository contains my work on the [cryptopals challenges][1], a set of programming challenges meant to teach the basics of real-world cryptography.

The bulk of the complexity is implemented in [`src/bytevector.cpp`][2] and [`src/Crypto.cpp`][5], which are a library I wrote to simplify handling strings of bytes in C++ and a basic cryptography library built on top of libssl's [AES-ECB][6] implementation, respectively.

The most interesting challenge, in my opinion, is [Challenge 17][3], which involves a padding oracle attack on [AES-CBC][7]. You can see my solution [here][4].

# usage

After cloning the repository, you can compile the challenges using make. For example, to compile challenge 13's code, you would write:

    $ make p13

This will generate the executable `p13` in the current directory.

If you want to compile all challenges, just run:

    $ make

Finally, to remove executables and intermediate build files, run:

    $ make clean

[1]: https://www.cryptopals.com
[2]: https://www.github.com/fletcherw/cryptopals/blob/master/src/bytevector.cpp
[3]: https://cryptopals.com/sets/3/challenges/17
[4]: https://www.github.com/fletcherw/cryptopals/blob/master/src/problem17.cpp
[5]: https://www.github.com/fletcherw/cryptopals/blob/master/src/Crypto.cpp
[6]: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_.28ECB.29
[7]: https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_Block_Chaining_.28CBC.29
