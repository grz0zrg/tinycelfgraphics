Collection of tiny C ELF programs with graphics output
=====

This repository is a collection of small ELF (Executable and Linkable Format) executables able to output graphics. All compiled with **GCC 7.5.0**.

The goal was to build a collection of very small standalone Linux graphics programs written in C with minimal amount of assembly (which is inline and mainly used for [syscall](https://en.wikipedia.org/wiki/System_call))

The rules was to be able to output & view graphics data, it also should be able to quit properly using Ctrl+C (SIGINT)

All of them compile down to less than 512 bytes with some less than 256 bytes (182 bytes to be exact excluding ASM version)

There is only one pure assembly framebuffer program to show how all of this compete against pure assembly and compliant ELF (maybe one could reach < 128b by dropping compliance) [credits](https://www.muppetlabs.com/~breadbox/software/tiny/return42.html)

It borrow several tricks from several sources mainly coming from the [Demoscene](https://en.wikipedia.org/wiki/Demoscene)

All C programs work in either 64 bits or 32 bits (must append `-m32` to GCC flags), 32 bits programs may be bigger or smaller.

This was used for my [Twigs](https://github.com/grz0zrg/twigs) 512 bytes procedural graphics intro.

## Build

Just go into any directory then into `src` directory and type `sh build.sh` this will invoke `make` multiple times for all defined width / height parameters in `build.sh` then all generated executables will go into the upper directory.

This was built with **GCC 7.5.0**

## How

* using GCC compiler optimizations through compiler flags (see Makefile of each programs)
* `-nostartfiles -nodefaultlibs` linker options among others (don't link to libc / don't use default startup code)
* usage of [strip](https://en.wikipedia.org/wiki/Strip_(Unix)) to remove various useless stuff from the executable binary (symbols, debug data etc.)
* usage of [sstrip](https://www.muppetlabs.com/~breadbox/software/elfkickers.html) to remove even more useless stuff that strip doesn't (section headers)
* clean more useless bits from the executable with a sed pass [from blackle](https://github.com/blackle/Tiny-X11-Trans-Flag)
* compress the ELF binary with LZMA (Note: this step + next step of course rely on some tools like **lzcat** **sed** which must be available on the OS; this is generally not an issue as they are available by defaults on most Linux OS)
* build a binary from a small shell script + the compressed binary, the shell script unpack and run itself, it unpack the executable to `/tmp/g`, make it executable and may perform additional stuff (such as outputting audio, clearing the terminal, exiting properly etc.)
* truncate some bytes left from the compression format (CRC32 checksum)

Some details / credits about the optimizations can be gathered [here](https://in4k.github.io/wiki/linux)

If the compression / shell script may feel like cheating one can still compile some programs (framebuffer, file) down to less than 256 bytes for file / fbdev output.

There is still some room to remove some bytes if one don't care about clearing the terminal output or exiting properly. (~11 bytes saved)

There is also the `-march=` GCC option which can have varying result with ~2 bytes gain.

`strace` is usefull on optimized binary to see any problems with the syscall

## Graphics output

Several methods are used to output graphics, of which :

* [Portable Pixmap](https://en.wikipedia.org/wiki/Netpbm#File_formats) file output
* fbdev; framebuffer (eg. `/dev/fb0`)
* SDL
* SDL2

GPU-only method using a GLSL fragment shader :
* [clutter](https://blogs.gnome.org/clutter/)

Framebuffer / file output is probably the most compatible option followed by SDL as i believe it is installed by default on many systems.

## Sound output

There is no sound output on the provided samples but it can be added easily by using `aplay` in the shell script (and piping data to it such as obviously `/dev/random`)

### File output

Only limited to static graphics data.

This output a `.ppm` (Portable Pixmap) image named `i` with a centered white pixel in current directory and call `xdg-open` to open it.

There is two versions of the program in `file.c` :

* standard version with open / write syscall, it is slightly larger than the one below
* (default) shortcut version which write the binary data to stdout (single write syscall) which is then redirected by the shell script to the `.ppm` file

64 bits ELF result :

* 190 bytes optimized + compressed

Note : 

* Due to PPM format some more bytes may be taken when the image resolution is increased.
* Some more bytes could be gained by calling `eog` or `feh` instead of `xdg-open` but it would probably reduce compatibility.
* The image should be named `.ppm` to be compatible with most files explorer.

### Framebuffer

This use the framebuffer device `/dev/fb0` (fbdev) to output graphics data. (white centered pixel)

The framebuffer device sometimes require that the user must either be in `video` or `tty` group otherwise the executable must be run by `root` or with `sudo`

The generated binary res / bit depth should match the framebuffer settings in order to work.`

64 bits ELF result :

* 240 bytes optimized
* 182 bytes optimized + compressed

Note : 176 bytes by removing null syscall parameters, this is probably safe on x86-64 but i don't know if it is safe for x86 platforms so i let that out.

### SDL

This use the SDL library to output graphics data. (white centered pixel)

Only two calls : `SDL_SetVideoMode` and `SDL_Flip`, `SDL_Init` call seem uneeded so it was left out.

Exiting properly was a bit tough through SDL calls so it was handled in the shell script by running the program in background, setting up a SIGINT trap which then kill the process.

64 bits ELF result :

* 969 bytes optimized
* 404 bytes optimized + compressed

Switching to 32 bits ELF actually free some more bytes :

32 bits ELF result :

* 374 bytes

### SDL2

This use the SDL2 library to output graphics data. (white centered pixel)

Unless the SDL version this one doesn't seem to require any additional setup to exit properly ?

64 bits ELF result :

* 1073 bytes optimized
* 456 bytes optimized + compressed

### Clutter (accelerated)

This use the Clutter libary to output graphics data through a GLSL fragment shader. (white screen)

64 bits ELF result :

* 1409 bytes optimized
* 464 bytes optimized + compressed

For anything 'modern' this is probably the best choice as it is accelerated. There is probably not enough room for anything complex in 512 bytes (at least on 64 bits) because Clutter GLSL symbols are long named.

Didn't try the 32 bits version but this probably save some more bytes.

A good source of fragment shaders to learn from is [Shadertoy](https://www.shadertoy.com)

### Framebuffer + pure assembly

64 bits assembly program that use the framebuffer device `/dev/fb0` (fbdev) to output graphics data. (white centered pixel)

This respect the ELF specification; better gains (~36 bytes) can be achieved by overlapping ELF headers or switching to 32 bits. See [tiny ELF](http://www.muppetlabs.com/~breadbox/software/tiny/return42.html)

64 bits ELF result :

* 196 bytes
* 173 bytes compressed

This is only ~9 bytes less than the C version! (and 3 bytes if syscall null values are discarded in the C program)

### More

Some more bytes can be gained by tweaking the ELF header, automatic tools [exists](https://git.titandemo.org/PoroCYon/norjohe/-/tree/master/src) to do that.