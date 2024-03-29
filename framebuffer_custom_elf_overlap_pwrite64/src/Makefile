CC=gcc
STANDARD_FLAGS=-std=c99 -pedantic -Wno-switch -Wno-conversion -Wno-uninitialized -Wno-strict-aliasing \
	-fno-exceptions -ffast-math -fsingle-precision-constant -fno-ident \
    -funsafe-math-optimizations -fvisibility=hidden -fmerge-all-constants \
    -fno-align-functions -fno-align-loops -fno-math-errno #-march=native
OPTI_FLAGS=-Os -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector -ftree-vectorize -fopt-info-vec-optimized \
	-fomit-frame-pointer -fno-math-errno -fdata-sections -ffunction-sections -fno-stack-check \
	-nostartfiles -nodefaultlibs -no-pie -fno-plt -fno-pic -fno-unroll-loops -Winline 
LINKER_FLAGS=-Wl,--gc-sections \
	-Wl,--build-id=none -z norelro -Wl,-z,noseparate-code -Wl,--no-eh-frame-hdr \
	-Wl,--no-ld-generated-unwind-info -Wl,--hash-style=sysv \
	-Wl,-z,nodynamic-undefined-weak
NAME=fb
LZMA_ARGS=--format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=32,depth=0,dict=16384 --keep

all:
	rm -f $(NAME).lzma
	rm -f payload
	rm -f payload.bin
	rm -f nm_output
	$(eval LD_BITS=$(shell [ $(BITS) = 32 ] && echo "-m elf_i386" || echo "-m elf_x86_64"))
	$(CC) -o $(NAME) $(NAME).c -DWIDTH=$(WIDTH) -DHEIGHT=$(HEIGHT) -m$(BITS) $(STANDARD_FLAGS) $(OPTI_FLAGS) $(LINKER_FLAGS)
	# ugly 'fix' from seabios for newer version of ld which will fail when linking one executable to another (see: http://sourceware-org.1504.n7.nabble.com/recent-ld-built-for-x86-64-fails-to-accept-16bit-code-as-input-seabios-build-td641750.html)
	# original comment: Change e_type to ET_REL so that it can be used to link. Unlike GNU ld, lld does not allow an ET_EXEC input.
	# ld 2.30 does not have the problem but 2.36.1 does
	# NOTE : the entry point may change due to that so $(ENTRY) below is completely wrong and it result in a crash on my machine (vs ld 2.30)
	#        don't know yet how to fix it cleanly except by just defining $(ENTRY) to be the address defined in the ld script (padded)... maybe check if that fix can be avoided ?
	#printf '\1' | dd of=$(NAME) conv=notrunc bs=1 seek=16 status=none
	# source : http://mainisusuallyafunction.blogspot.com/2015/01/151-byte-static-linux-binary-in-rust.html
	# 'smash all the sections together, with no alignment padding, then extract that section as a headerless binary blob'
	ld --gc-sections $(LD_BITS) -e _start -T script_$(BITS).ld -o payload $(NAME)
	objcopy -j combined -O binary payload payload.bin
	# determine the entry point address
	nm -f posix payload | grep '^_start ' | awk '{print $$3}' > nm_output
	# data offset (hex and uppercase) if any data is defined before the payload in elf64.s or elf32.s an offset must be added corresponding to the size of the data (so it compute the correct entry point)
	$(eval OFFSET=-5)
	# get entry point address from nm_output (uppercase hex), compute offset if any and pad the result
	$(eval NM_OUTPUT=$(shell cat nm_output | tr '[a-z]' '[A-Z]'))
	$(eval NM_OUTPUT=$(shell perl -e 'print sprintf "%016s\n","$(NM_OUTPUT)"'))
	$(eval ENTRY=$(shell echo "obase=16;ibase=16;$(NM_OUTPUT) + $(OFFSET)" | bc))
	$(eval ENTRY=$(shell perl -e 'print sprintf "%016s\n","$(ENTRY)"'))
	# build the final binary from the binary and custom ELF header
	nasm -f bin -o $(NAME) -D width=$(WIDTH) -D height=$(HEIGHT) -D entry=0x$(ENTRY) elf$(BITS).s
	wc -c $(NAME)
	# comment the following three lines if you want raw ELF file (no stub / shell script / LZMA compression)
	#lzma $(LZMA_ARGS) $(NAME)
	#cat unpack_lzma.header $(NAME).lzma > $(NAME)
	#truncate -s -4 $(NAME)
	chmod +x $(NAME)
	rm -f $(NAME).lzma
	rm -f payload
	rm -f payload.bin
	wc -c $(NAME)
	mv $(NAME) ../$(NAME)_$(WIDTH)x$(HEIGHT)x$(BITS)
	chmod +x ../$(NAME)_$(WIDTH)x$(HEIGHT)x$(BITS)
