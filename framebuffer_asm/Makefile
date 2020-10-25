NAME=fb
LZMA_ARGS=--format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=32,depth=9,dict=16384 --keep
GZIP_ARGS=-cnk9
ZIP_ARG=-cnk9

all: build
	lzma $(LZMA_ARGS) $(NAME)
	cat unpack_lzma.header $(NAME).lzma > $(NAME)
	chmod +x $(NAME)
	rm $(NAME).lzma
	wc -c $(NAME)

build:
	nasm -fbin -o $(NAME) $(NAME).asm
	chmod +x $(NAME)
	wc -c $(NAME)
