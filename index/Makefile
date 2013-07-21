LIB_PATH=$(CURDIR)/../lib/sdsl-lite

CC=g++
CC_FLAGS=-Wall -g -O3 -std=c++0x -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -msse4.2
#CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -ldl -ltcl -lefence
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64

all: build_index suffixnode document base_util
	$(CC) $(CC_FLAGS) ./base_util.o ./Document.o ./BasicSuffixTree.o ./build_index.o -o ../bin/build_index.x $(CCLIB)

build_index:
	$(CC) $(CC_FLAGS) -c build_index.cpp $(CCLIB) 

suffixnode:
	$(CC) $(CC_FLAGS) -c BasicSuffixTree.cpp $(CCLIB)

document:
	$(CC) $(CC_FLAGS) -c Document.cpp $(CCLIB)

base_util:
	$(CC) $(CC_FLAGS) -c base_util.cpp $(CCLIB)

clean:
	rm -f *.o
	rm -f ../index/build_index
