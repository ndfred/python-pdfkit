CFLAGS:=-g -Wall -Wextra -pedantic -std=c99
CC:=gcc $(CFLAGS)

all: count

%.o: %.c
	$(CC) -c $<

count: count.o
	$(CC) -framework Foundation -framework ApplicationServices $^ -o $@

test-count: count PDFKitGuide.pdf
	./count PDFKitGuide.pdf

PDFKitGuide.pdf:
	curl http://developer.apple.com/mac/library/documentation/GraphicsImaging/Conceptual/PDFKitGuide/PDFKitGuide.pdf -o PDFKitGuide.pdf

clean:
	rm -f count count.o

distclean: clean
	rm -f PDFKitGuide.pdf
