C=g++
CFLAGS=-std=c99 -O3
all: bwtsearch

bwtsearch: bwtsearch.cpp bwtsearch.h
	$(C) bwtsearch.cpp -o bwtsearch

clean:
	rm bwtsearch