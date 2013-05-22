/*
  Author: Bruce Kuo
  Date: 2013.05.22

  Description:
    This file is to do query processing
*/

#include <stdio.h>
#include <string.h>

#include <set>

#include <sdsl/suffix_trees.hpp>
#include <sdsl/util.hpp>

using std;
using sdsl;

typedef cst_sada<> tCST;

struct meta_node {
  int id, pos;

  meta_node(){}
  meta_node(int _id, int _pos): id(_id), pos(_pos){}

  bool operator < (const meta_node &t) const {
    return id < t.id;
  }
} ;

bool load_metadata(vector<int> &metadata, char *filename) {
  FILE *fp = fopen(filename, "r");
  int x, y;
  while( fscanf(fp, "%d%d", &x, &y) != EOF )
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

  char s_query[510];
  while(1) {
    fprintf(stdout, "Please enter the query string:\n");
    if( !gets(s_query) )  break;
    //
    // TODO (Bruce Kuo):
    //  Add translate to integer functions
    //
    int length = strlen(s_query);

    //
    // TODO (Bruce Kuo):
    //  (1) compare with forward search and backword search
    //      now we use forward search
    //  (2) now we assume that the number of query words is less than 8
    //
    node_t v = cst.root();
    string query = string(s_query);
    string::iterator it = query.begin();
    for(uint64_t pos=0;it != query.end();++it) {
      if(forward_search(cst, v, it-query.begin(), *it, pos) > 0) {
        
      } else {
        break;
      }
    }
    int node_id = v.id();


  }

  return 0;
}

