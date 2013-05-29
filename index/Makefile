LIB_PATH=/home/bruce3557/SuffixTreeBased/OurIndex/lib/sdsl-lite

CC=g++
CC_FLAGS=-Wall -g -O3 -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -msse4.2 -std=c++0x
#CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -ldl -ltcl -lefence
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64

all: 
	$(CC) $(CC_FLAGS) -o ../bin/build_index build_index.cpp $(CCLIB) 

clean:
	rm -f *.o
	rm -f ./build_index
