LIB_PATH=$(CURDIR)/../lib/sdsl-lite

CC=g++
CC_FLAGS=-O3 -I$(LIB_PATH)/include -L$(LIB_PATH)/lib -DNDEBUG -msse4.2 -std=c++11
CC_LIB=-lsdsl -ldivsufsort -ldivsufsort64

DOCUMENT=../index/Document.o
SUFFIXNODE=../index/BasicSuffixTree.o
BASEUTIL=../index/base_util.o

all: query-engine

query-engine: document query-process suffixnode base_util
	$(CC) $(CC_FLAGS) -o ../bin/query_engine query_processing.o ../index/Document.o ../index/BasicSuffixTree.o ../index/base_util.o $(CC_LIB)

query-process:
	$(CC) $(CC_FLAGS) -c ./query_processing.cpp $(CC_LIB)

document:
ifeq ($(wildcard $(DOCUMENT)),)
	$(CC) $(CC_FLAGS) -c ../index/Document.cpp $(CC_LIB)
endif

suffixnode:
ifneq '$(SUFFIXNODE)' '$(wildcard $(SUFFIXNODE))'
	$(CC) $(CC_FLAGS) -c ../index/BasicSuffixTree.cpp $(CC_LIB)
endif

base_util:
ifneq  '$(BASEUTIL)' '$(wildcard $(BASEUTIL))'
	$(CC) $(CC_FLAGS) -c ../index/base_util.cpp $(CC_LIB)
endif

clean:
	rm -rf *.o
	rm -rf ../bin/query_engine
