Collection of tiny C ELF programs with graphics output
=====

This repository is a collection of small ELF (Executable and Linkable Format) executables able to output graphics. All compiled with **GCC 7.5.0**.

The goal was to build a collection of very small standalone Linux graphics programs written in C with minimal amount of assembly (which is inline and mainly used for [syscall](https://en.wikipedia.org/wiki/System_call))

The rules was to be able to output & view graphics data, it also should be able to quit properly using Ctrl+C (SIGINT)

All of them compile down to less than 512 bytes with some less than 256 bytes (182 bytes to be exact excluding ASM version and unsafe fb version with custom ELF headers (158 bytes!))

There is only one pure assembly framebuffer program to show how all of this compete against pure assembly and 'compliant' ELF (maybe one could reach < 128b by dropping compliance) [credits](https://www.muppetlabs.com/~breadbox/software/tiny/return42.html)

It borrow several tricks from several sources mainly coming from the [Demoscene](https://en.wikipedia.org/wiki/Demoscene)

All pure C programs work in either 64 bits or 32 bits (must append `-m32` to GCC flags), 32 bits programs may be bigger or smaller.

This was used for my [Twigs](https://github.com/grz0zrg/twigs) 512 bytes procedural graphics intro.

The best method for anything real-time is probably the "Framebuffer with custom ELF headers" (generated binary size is the same as pure assembly and it allow more controls over the ELF header)

## Why

Fun, portability, readability, accessibility.

It is mainly targeted at [sizecoding](http://www.sizecoding.org/wiki/Main_Page) stuff. (art of creating very tiny programs)

* Portability : There is some inline assembly but it is still way more portable than pure assembly.
* Readability : Not always true but generally it is, when doing sizecoding the code can get pretty weird / cryptic, with C it is more straightforward.
* Accessibility : Allow to get into sizecoding quickly with common programming language.

As a major downside there is less controls over the generated code and maybe some odd behaviors due to compiler bugs etc.

## Build

Just go into any directory then into `src` directory and type `sh build.sh` this will invoke `make` multiple times for all defined width / height parameters in `build.sh` then all generated executables will go into the upper directory.

This was built with **GCC 7.5.0**

## How

applicable to all except C + custom ELF header and ASM version :

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

There is still some room to remove some bytes if one don't care about clearing the terminal output or exiting properly. (~11 bytes to most)

There is also the `-march=` GCC option which can have varying result with ~2 bytes gain.

When compressed changing some constants can sometimes lead to some gain (depend on content), for example switching some fields of the ELF header to 0 can lead to ~8 bytes gain, this may be no more ELF compliant though.

`strace` is usefull on optimized binary to see any problems with the syscall

There is also a framebuffer version with GCC output + custom ELF headers merged together to form a small binary, it is probably the best of all methods here.

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

There is no sound output on the provided examples but it can be added easily by using `aplay` in the shell script (and piping data to it such as obviously `/dev/random`)

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

Note : 

* 176 bytes by removing null syscall parameters, this is probably safe on x86-64 but i don't know if it is safe for x86 platforms so i let that out.
* for static graphics (procedural) some bytes can be gained by using a static buffer + call to single write syscall + adjusting the shell script to output to /dev/fb0 just like the "file output" example

### Framebuffer with custom 64 bits ELF headers

Same as before with a custom 64 bits assembly ELF header, probably the best of all methods due to flexibility and size of generated binary. Can be adapted for file output.

The main advantage over all methods here is : C code + hand made ELF header customizations / complete controls

The main disadvantage is : it can be harder to use since some sections like .rodata are left out so for example any float constants in C code don't work as-is, they must be defined somewhere in the assembly code and referenced in C code through pointers (see sources) if you only use integers in your program it should work as-is.

How ? The program is compiled with GCC (with optimization flags), a binary blob (without headers) is then extracted and included inside a custom ELF header compiled with NASM, the result is then compressed.

There is some potentially unsafe shortcuts compared to others (they are not mandatory) such as :

* ELF padding / ABI target / version field is used to store the framebuffer device (/dev/fb0) string, using the padding field is safe but there is still doubts about the ABI field
* Syscalls null arguments are discarded (see `fb.c` comments), this rely on the asumption that all registers are set to 0 when the program start.

64 bits ELF result (1920x1080) :

* 171 bytes optimized
* 158 bytes optimized + compressed

32 bits version does not work yet. (crashing, why ?)

Original idea / implementation come from [this article](http://mainisusuallyafunction.blogspot.com/2015/01/151-byte-static-linux-binary-in-rust.html)

Note : The resulting binary can be disassembled with `objdump -b binary -D -m i386:x86-64 binary_name`

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
* 169 bytes compressed

This is only ~13 bytes less than the C version! (and ~7 bytes if syscall null values are discarded in the C program)

Note : this is the same size as the Framebuffer custom ELF, there is some small size differences due to the added loop.

### More

Some more bytes can be gained by tweaking the ELF header, this can be highly tricky / unsafe.

GCC version could also influence the final size (for the examples above recent GCC versions (up to 11) does not affect the binary size)