# DOOM port for PolkaVM

This is a DOOM port for [PolkaVM](https://github.com/koute/polkavm).

It's based on [doomgeneric](https://github.com/ozkl/doomgeneric), but it's even more portable.
The source code is completely standalone and doesn't even require a libc. It also has out-of-box
support for audio due to built-in OPL emulation, which doomgeneric doesn't have.

## Building

1. Get a clang for RV32E
	- Upstream LLVM from version 18.X and above support the RV32E target
	- Scripts to build a toolchain are [available here](https://github.com/paritytech/rustc-rv32e-toolchain) or [here](https://github.com/xermicus/revive/blob/main/build-llvm.sh)
2. Make sure that the compatible LLVM build is found via `$PATH`. Verify: `clang --target=riscv32 -print-supported-extensions 2>&1 | grep 'RV{32,64}E'`
3. Run `./build.sh`.
4. Run `polkatool link -s output/doom.elf -o output/doom.polkavm`.

## License

GPLv2+

Many third-party libraries are used here with varying licenses; see the `libs` directory for details.
