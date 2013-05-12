LIB_PATH=/home/bruce3557/SuffixTreeBased/OurIndex/lib/sdsl-lite

CC=g++
CC_FLAGS=-Wall -g -O3 -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -funroll-loops -msse4.2 -I/home/bruce3557SuffixTreeBased/OurIndex/lib/IntervalTree/ -std=c++0x
#CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -ldl -ltcl -lefence
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -ldl -ltcl -lefence

all: BasicSuffixTree Document BuildIndex BaseUtil
	$(CC) $(CC_FLAGS) -o build_index $(CCLIB) base_util.o BasicSuffixTree.o Document.o build_index.o ../lib/IntervalTree/interval_tree.o

BuildIndex: build_index.cpp
	$(CC) $(CC_FLAGS) -c build_index.cpp $(CCLIB)

BasicSuffixTree: BasicSuffixTree.cpp BasicSuffixTree.h
	$(CC) $(CC_FLAGS) -c BasicSuffixTree.cpp $(CCLIB)

Document: Document.cpp Document.h
	$(CC) $(CC_FLAGS) -c Document.cpp $(CCLIB)

BaseUtil: base_util.h base_util.cpp
	$(CC) -O3 -Wall -g -c base_util.cpp

clean:
	rm -f *.o
	rm -f ./build_index
