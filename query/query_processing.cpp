/*
  Author: Bruce Kuo
  Date: 2013.05.22

  Description:
    This file is to do query processing
*/

// C Library
#include <stdio.h>
#include <string.h>

// C++ Library
#include <string>
#include <set>

// SDSL library
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
    if(id == -1)  return false;
    return id < t.id;
  }
} ;

bool load_metadata(set< meta_node > &metadata, char *filename) {
  FILE *fp = fopen(filename, "r");
  int x, y;
  while( fscanf(fp, "%d%d", &x, &y) != EOF )
    metadata.push_back( meta_node(y, x) );
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

  //vector<int> metadata;
  set< meta_node > metadata;
  if( !load_metadata(metadata, "metadata.idx") ) {
    fprintf(stderr, "Loading metadata error...");
    return 0;
  }

  FILE *p_node_info = fopen("node_info.idx", "r");
  if( !p_node_info ) {
    fprintf(stderr, "Loading node information error...");
    return 0;
  }

  char s_query[510];
  while(1) {
    long long start_time, end_time;
    int topk;
    fprintf(stdout, "Please enter the starting time:\n");
    scanf("%lld", &start_time);
    fprintf(stdout, "Please enter the ending time:\n");
    scanf("%lld", &end_time);
    gets(s_query);
    fprintf(stdout, "Please enter the query string:\n");
    if( !gets(s_query) )  break;
    fprintf(stdout, "Please enter the top-k version you want to get:\n");
    fprintf(stdout, "enter -1 if you want to use default, default: 10\n");
    scanf("%d", &topk);
    if(topk == -1)  topk = 10;

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

    // Find the word that save in the suffix node
    set< meta_node >::iterator it;
    while( 1 ) {
      int node_id = v.id();
      it = metadata.find( meta_node(node_id, -1) );
      if( it != metadata.end() )  break;
      v = cst.parent(v);
    }

    SuffixTreeNode node_info;
    if( it != metadata.end() && (it->id) != -1 ) {
      // move the pointer to the node starting position
      int st_pos = it->pos;
      ++it;
      int ed_pos = it->pos;
      fseek(p_node_info, st_pos, SEEK_SET);
      node_info.load(p_node);
    } else {
      // TODO (Bruce Kuo):
      //  Dummy node implementation
    }

    // TODO (Bruce Kuo):
    //  add top-k extraction
    string answer = node_info.query(start_time, end_time, topk);
    fprintf(stdout, "%s\n", answer.c_str());
    
  }

  return 0;
}

