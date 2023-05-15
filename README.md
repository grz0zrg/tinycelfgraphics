Collection of tiny C ELF programs with graphics output
=====

This repository is a collection of small ELF (Executable and Linkable Format) executables able to output graphics. All compiled with **GCC 7.5.0**.

The goal was to build a collection of very small standalone Linux graphics programs written in C with minimal amount of assembly (which is inline and mainly used for [syscall](https://en.wikipedia.org/wiki/System_call))

The rules was to be able to output & view graphics data, it also should be able to quit properly using Ctrl+C (SIGINT)

All of them compile down to less than 512 bytes with some less than 256 bytes and two (32 bits only) less than 128 bytes (85 bytes !)

There is only one 100% assembly framebuffer program to show how all of this compete against pure assembly and 'compliant' ELF [credits](https://www.muppetlabs.com/~breadbox/software/tiny/return42.html)

It borrow several tricks from several sources mainly coming from the [Demoscene](https://en.wikipedia.org/wiki/Demoscene)

All pure C programs work in either 64 bits or 32 bits (must append `-m32` to GCC flags), 32 bits programs may be bigger or smaller.

This was used for [my procedural graphics 128b/256b intro](https://www.pouet.net/groups.php?which=15005) although some are pure assembly.

The best method for anything real-time is probably the "Framebuffer with custom ELF headers" (generated binary size is the same as pure assembly, it also allow full controls over the ELF header and other ASM parts)

A good and up to date source for sizecoding on Linux (also show how to output sounds and more): [http://www.sizecoding.org/wiki/Linux](http://www.sizecoding.org/wiki/Linux)

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
* compress the ELF binary with LZMA (Note: this step + next step of course rely on some tools like **lzcat** **sed** which must be available on the OS; this is generally not an issue as they are available by default on most Linux OS)
* build a binary from a small shell script + the compressed binary, the shell script unpack and run itself, it unpack the executable to `/tmp/g`, make it executable and may perform additional stuff (such as outputting audio, clearing the terminal, exiting properly etc.)
* truncate some bytes left from the compression format (CRC32 checksum)

Some details / credits about the optimizations can be gathered [here](https://in4k.github.io/wiki/linux)

If the compression / shell script may feel like cheating one can still compile some programs (framebuffer, file) down to less than 256 bytes (even 128 bytes) for file / fbdev output.

There is still some room to remove some bytes if one don't care about clearing the terminal output or exiting properly. (~11 bytes to most)

There is also the `-march=` GCC option which can have varying result on binary size.

When compressed changing some constants can sometimes lead to some gain (depend on content), for example switching some fields of the ELF header to 0 can lead to ~8 bytes gain, this may be no more ELF compliant though.

`strace` is usefull on optimized binary to see any problems with the syscall

There is also a framebuffer version with GCC output + custom ELF headers merged together to form a small binary, it is probably the best of all methods here.

## Graphics output

Several methods are used to output graphics, of which :

* [Portable Pixmap](https://en.wikipedia.org/wiki/Netpbm#File_formats) file output
* fbdev; framebuffer (eg. `/dev/fb0`), open + mmap OR open + pwrite64 using the stack as buffer (best for realtime / feedback effects)
* SDL
* SDL2

GPU-only method using a GLSL fragment shader :
* [clutter](https://blogs.gnome.org/clutter/)

Framebuffer / file output is probably the most compatible option followed by SDL as i believe it is installed by default on many systems.

## Sound output

There is no sound output on the provided examples but it can be added easily by using `aplay` in the shell script (and piping data to it such as obviously `/dev/random`)

Here is some [code](http://www.sizecoding.org/wiki/Linux)

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

### Framebuffer with custom 32 / 64 bits ELF headers

Same as before with a custom 32 / 64 bits assembly ELF header, probably the best of all methods due to flexibility and size of generated binary. Can be adapted for file output easily.

The main advantage over all methods here is : C code + hand made ELF header customizations / complete controls

The main disadvantage is : it can be harder to use since some sections like .rodata are left out so for example any float constants in C code don't work as-is, they must be defined somewhere in the assembly code and referenced in C code through pointers (see sources) **if you only use integers** in your program it should work as-is.

Another (small) disadvantage is less portability, you may have to rewrite the header for each platforms.

How ? The program is compiled with GCC (with optimization flags), a binary blob (without headers) is then extracted and included inside a custom ELF header compiled with NASM, the result is then compressed.

There is some potentially unsafe shortcuts compared to others (they are not mandatory) such as :

* ELF padding / ABI target / version field is used to store the framebuffer device (/dev/fb0) string
* Syscall null arguments are discarded (see `fb.c` comments), this rely on the asumption that all registers are set to 0 when the program start.

32 bits ELF result (1920x1080) :

* 165 bytes optimized
* 196 bytes optimized + compressed (why ?)

64 bits ELF result (1920x1080) :

* 171 bytes optimized
* 164 bytes optimized + compressed (153 bytes if the console isn't cleared / program doesn't exit properly)

Original idea / implementation come from [this article](http://mainisusuallyafunction.blogspot.com/2015/01/151-byte-static-linux-binary-in-rust.html)

Note : Non compressed version does not quit properly

Note : See `build.sh` to target 32 or 64 bits platform (run it twice if you switch because there is some unsolved issues in how variables are handled in the makefile)

Note : The resulting 64 bits binary can be disassembled with `objdump -b binary -D -m i386:x86-64 binary_name`

### Framebuffer with custom ELF headers + fields overlap

Same as before except some ELF header fields are overlapped (31 bytes gain for 64 bits version)

Note : This does not respect the ELF specification so potentially unsafe.

Note : 32 bits version goes further than just overlapping headers by integrating bits of [Whirlwind Tutorial](http://www.muppetlabs.com/~breadbox/software/tiny/teensy.html) it is also maybe more unsafe than 64 bits due to some shortcuts (e_sh* values are actual code + fb0 string null byte on e_ehsize)

32 bits ELF result (1920x1080) :

* **127 bytes** optimized
* 180 bytes optimized + compressed

64 bits ELF result (1920x1080) :

* 140 bytes optimized
* 159 bytes optimized + compressed (148 bytes if the console isn't cleared / program doesn't exit properly)

Compression does not seem to help anymore so it is disabled by default. (see `Makefile` to enable it)

Note : Some potentially unsafe tricks can be used to gain ~4 bytes for the 32 bits ELF by tweaking the `sys_mmap` function (no push / pop + single movl and one arg) bringing the 32 bits ELF to around **123 bytes**

Some more bytes can be gained for the 32 bits version by hand coding some stuff in assembly (see next) but at this point it is probably better to go for pure assembly. :)

### Framebuffer with custom ELF headers + fields overlap + x86 framebuffer init (open / mmap)
 
Same as before except the framebuffer initialization (fopen / mmap calls) is hand coded; 32 bits x86 only

open/mmap disadvantage : reading from the mmaped memory is very slow, can also get tearing and may be hard to do feedback effects, see the pwrite64 below for fixes

32 bits ELF result (1920x1080) :

* **85 bytes**

It use optimized framebuffer initialization code from [lintro](https://www.pouet.net/prod.php?which=58560) a 128 bytes intro by frag/fsqrt

This is probably one of the tiniest a C fbdev program can be although there is still some tricks to gain ~3 bytes such as changing entry point (see [lintro sources](https://www.pouet.net/prod.php?which=58560))

Ideal for 128 bytes intro although at this point it is probably better to ditch C and do pure assembly :)

### Framebuffer with custom ELF headers + fields overlap + x86 framebuffer init (open / pwrite64 + stack usage)
 
Same as before except the framebuffer initialization (fopen / pwrite64 calls) is hand coded and some room is made on the stack for the buffer; 32 bits x86 only

open/pwrite64/stack advantage : fast read, no tearings, allow feedback effects, quite easy to do scrolling and glitchy effects by using the pwrite64 parameters

open/pwrite64/stack disadvantage : sligthly larger, use the stack as a buffer so if your C program use the stack some data may go into the display buffer, it may crash if you write data that the C program use (can be mitigated with some offset)

Another disadvantage of using the stack is that the stack size is sometimes limited to 8Mb, it is probably safe to hold a 1920x1080 32 bits framebuffer but maybe not more.

note : the pwrite64 is hand coded in the C program because it is easy to embed it into a loop that way (youll have to call it each time you want to draw)

Once again it is probably better to ditch C and do pure assembly at this point :)

32 bits ELF result (1920x1080) :

* **87 bytes**

Idea based on [http://www.sizecoding.org/wiki/Linux](http://www.sizecoding.org/wiki/Linux) there is some more tricks on there!

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

This use the Clutter libary to output graphics data through a GLSL fragment shader. (a window filled with red)

The Clutter package should be installed in popular distributions, if not the package to install on Ubuntu is : `sudo apt install libclutter-1.0-dev`

64 bits ELF result :

* 1417 bytes optimized
* 503 bytes optimized + compressed

Probably the best choice for "modern" graphics as it is accelerated. There is probably not enough room for anything complex in 512 bytes (at least on 64 bits) because Clutter GLSL symbols are long named.

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

Conclusion : For 128b / 256b programs GCC can do a sufficient job but it highly depend on the code, GCC may use the stack heavily which is sometimes not good for sizecoding, it will not beat hand optimized assembly which may at least have 25% less bytes.

### More

Some more bytes can be gained by tweaking the ELF header (see overlap example), this can be highly tricky / unsafe. Some more can be gained with compiler options such as `-fomit-frame-pointer` `-march=`; depend on code.

Also highly recommend to disassemble and analyze the resulting binary `objdump -b binary -D -m i386 ../fb_1920x1080x32`, GCC may put some useless setup bits (for stack alignment etc.) in there just before entering main like some useless `push` (once had 7 bytes gain by looking at that!).

GCC version can also influence the binary size, Clutter sample for instance has some bytes difference between GCC 7, 8 and 12. On GCC-12 the `-Oz` option is available and may help to get some more bytes.