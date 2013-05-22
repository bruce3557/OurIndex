/*
  Author: Bruce Kuo
  Date: 2013.05.22

  Description:
    This file is to do query processing
*/

#include <stdio.h>
#include <string.h>

#include <sdsl/suffix_trees.hpp>
#include <sdsl/util.hpp>

using std;
using sdsl;

typedef cst_sada<> tCST;

bool load_metadata(vector<int> &metadata, char *filename) {
  FILE *fp = fopen(filename, "r");
  int x;
  while( fscanf(fp, "%d", &x) != EOF )
    metadata.push_back(x);
  fclose(fp);
}

int main(int argc, char *argv[]) {
  if( argc < 3 ) {
    PrintUsage(argv);
    return 0;
  }

  tCST cst;
  if( !load_from_flie(cst, string("cst.idx")) ) {
    fprintf(stderr, "There is no data index, please build the index");
    return 0;
  }

  vector<int> metadata;
  if( !load_metadata(metadata, "metadata.idx") ) {
    fprintf(stderr, "Loading metadata error...");
    return 0;
  }

  return 0;
}

