CFLAGS:=-g -Wall -Wextra -pedantic -std=c99
CC:=gcc $(CFLAGS)

all: count thumbnails extract
test: test-count test-thumbnails test-extract

%.o: %.c
	$(CC) -c $<

count: count.o
	$(CC) -framework Foundation -framework ApplicationServices $^ -o $@

test-count: count PDFKitGuide.pdf
	./count PDFKitGuide.pdf

thumbnails: thumbnails.o
	$(CC) -framework Foundation -framework ApplicationServices $^ -o $@

test-thumbnails: thumbnails PDFKitGuide.pdf
	./thumbnails PDFKitGuide.pdf

extract: extract.o
	$(CC) -framework Foundation -framework ApplicationServices $^ -o $@

test-extract: extract PDFKitGuide.pdf
	./extract PDFKitGuide.pdf

PDFKitGuide.pdf:
	curl http://developer.apple.com/mac/library/documentation/GraphicsImaging/Conceptual/PDFKitGuide/PDFKitGuide.pdf -o PDFKitGuide.pdf

clean:
	rm -f count count.o thumbnails thumbnails.o *.png

distclean: clean
	rm -f PDFKitGuide.pdf
