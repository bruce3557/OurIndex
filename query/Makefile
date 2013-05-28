LIB_PATH=/home/bruce3557/SuffixTreeBased/OurIndex/lib/sdsl-lite

CC=g++
CC_FLAGS=-Wall -g -O3 -pedantic -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -funroll-loops -msse4.2 -std=c++0x
CC_LIB=-lsdsl -ldivsufsort -ldivsufsort64

all: query-engine

query-engine:
	$(CC) $(CC_FLAGS) -o query_engine $(CC_LIB) query_processing.cpp

