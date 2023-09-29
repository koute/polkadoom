# DOOM port for PolkaVM

This is a DOOM port for [PolkaVM](https://github.com/koute/polkavm).

It's based on [doomgeneric](https://github.com/ozkl/doomgeneric), but it's even more portable.
The source code is completely standalone and doesn't even require a libc. It also has out-of-box
support for audio due to built-in OPL emulation, which doomgeneric doesn't have.

## Building

1. Build and install clang for RV32E. (Scripts to build a toolchain are [available here](https://github.com/koute/rustc-rv32e).)
2. Run `./build.sh`.
3. Run `polkatool link -s output/doom.elf -o output/doom.polkavm`.

## License

GPLv2+

Many third-party libraries are used here with varying licenses; see the `libs` directory for details.
