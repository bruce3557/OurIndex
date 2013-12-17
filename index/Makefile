LIB_PATH=$(CURDIR)/../lib/sdsl-lite

CC=g++
#CC_FLAGS=-Wall -g -O2 -std=c++11 -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -msse4.2
CC_FLAGS=-std=c++11 -Wall -DNDEBUG  -O2 -ffast-math -funroll-loops -msse4.2 -g -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib
#CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -ldl -ltcl -lefence
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64

all: build_index rtree_build_index suffixnode document base_util

build_index: suffixnode document base_util
	$(CC) $(CC_FLAGS) -c build_index.cpp $(CCLIB) 
	$(CC) $(CC_FLAGS) ./base_util.o ./Document.o ./BasicSuffixTree.o ./build_index.o -o ../bin/build_index.x $(CCLIB)
#	$(CC) $(CC_FLAGS) -c rtree_build_index.cpp $(CCLIB)

rtree_build_index: rsuffix_node document base_util
	$(CC) $(CC_FLAGS) -c rtree_build_index.cpp $(CCLIB)
	$(CC) $(CC_FLAGS) ./base_util.o ./Document.o ./BasicSuffixTree.o ./RSuffixTree.o ./rtree_build_index.o -o ../bin/rtree_index.x ($CCLIB)

suffixnode:
	$(CC) $(CC_FLAGS) -c BasicSuffixTree.cpp $(CCLIB)
#	$(CC) $(CC_FLAGS) -c RSuffixTree.cpp $(CCLIB)

rsuffix_node:
	$(CC) $(CC_FLAGS) -c RSuffixTree.cpp $(CCLIB)

document:
	$(CC) $(CC_FLAGS) -c Document.cpp $(CCLIB)

base_util:
	$(CC) $(CC_FLAGS) -c base_util.cpp $(CCLIB)

clean:
	rm -f *.o
	rm -f ../index/build_index.x
	rm -f ../index/rtree_index.x
