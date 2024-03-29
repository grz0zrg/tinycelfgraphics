CC=gcc
STANDARD_FLAGS=-std=c99 -pedantic -Wno-switch -Wno-conversion -Wno-uninitialized -Wno-strict-aliasing \
	-fno-exceptions -ffast-math -fsingle-precision-constant -fno-ident \
    -funsafe-math-optimizations -fvisibility=hidden -fmerge-all-constants \
    -fno-align-functions -fno-align-loops -fno-math-errno #-march=native
OPTI_FLAGS=-Os -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector -ftree-vectorize -fopt-info-vec-optimized \
	-fomit-frame-pointer -fno-math-errno -fdata-sections -ffunction-sections -fno-stack-protector -fno-stack-check \
	-nostartfiles -nodefaultlibs -no-pie -fno-plt -fno-pic -fno-unroll-loops -Winline 
LINKER_FLAGS=-Wl,--gc-sections \
	-Wl,--build-id=none -z norelro -Wl,-z,noseparate-code -Wl,--no-eh-frame-hdr \
	-Wl,--no-ld-generated-unwind-info -Wl,--hash-style=sysv \
	-Wl,-z,nodynamic-undefined-weak -Wl,--strip-all
NAME=clutter
# https://github.com/BR903/ELFkickers
SSTRIP=~/ELFkickers/sstrip/sstrip
LZMA_ARGS=--format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=32,depth=9,dict=16384 --keep

all:
	$(CC) -o $(NAME) $(NAME).c -DWIDTH=$(WIDTH) -DHEIGHT=$(HEIGHT) $(STANDARD_FLAGS) $(OPTI_FLAGS) `pkg-config clutter-1.0 --cflags --libs` $(LINKER_FLAGS)
	strip -R .note -R .data -R .bss -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .rela.dyn -R .shstrtab -R .gnu.hash $(NAME)
	$(SSTRIP) -z $(NAME)
	# clear useless bits (taken from blackle Tiny X11 Trans Flag)
	sed -i 's/_edata/\x00\x00\x00\x00\x00\x00/g' $(NAME);
	sed -i 's/__bss_start/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $(NAME);
	sed -i 's/_end/\x00\x00\x00\x00/g' $(NAME);
	#
	wc -c $(NAME)
	# compress & stub
	lzma $(LZMA_ARGS) $(NAME)
	cat unpack_lzma.header $(NAME).lzma > $(NAME)
	chmod +x $(NAME)
	rm $(NAME).lzma
	# remove CRC32 (4 bytes)
	truncate -s -4 $(NAME)
	# truncate some more bytes (NOTE : may works but unsafe)
	#truncate -s -1 $(NAME)
	wc -c $(NAME)
	mv $(NAME) ../$(NAME)_$(WIDTH)x$(HEIGHT)x32
