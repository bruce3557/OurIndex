LIB_PATH=$(CURDIR)/../lib/sdsl-lite

CC=g++
CC_FLAGS=-O3 -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -msse4.2 -std=c++0x
CC_LIB=-lsdsl -ldivsufsort -ldivsufsort64

all: query-engine

query-engine:
	$(CC) $(CC_FLAGS) -o ../bin/query_engine query_processing.cpp $(CC_LIB)

clean:
	rm -rf *.o
	rm -rf query_engine
