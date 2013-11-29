/*
  Author: Bruce Kuo
  Date: 2013.05.22

  Description:
    This file is to do query processing
*/

// C Library
#include <cstdio>
#include <cstring>

// C++ Library
#include <string>
#include <set>

// SDSL library
#include <sdsl/suffix_trees.hpp>

#include "../index/Document.h"
#include "../index/BasicSuffixTree.h"

using std::string;
using std::set;

using namespace sdsl;

typedef cst_sada<csa_bitcompressed<int_alphabet<> >> tCST;

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
  while( fscanf(fp, "%d%d", &x, &y) != EOF ) {
    metadata.insert( meta_node(y, x) );
  }
  fclose(fp);
  return true;
}


void PrintUsage(char *arg) {
  printf("Usage: type the command %s\n Then following it will tell you the step\n", arg);
}

int main(int argc, char *argv[]) {
  if( argc < 1 ) {
    PrintUsage(argv[0]);
    return 0;
  }

  tCST cst;
  if( !load_from_file(cst, string("cst.idx")) ) {
    fprintf(stderr, "There is no data index, please build the index");
    return 0;
  }

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

  memory_manager::use_hugepages();
  printf("AFTER LOADING...\n");
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
    tCST::node_type v = cst.root();
    //string query = string(s_query);
    //string::iterator qit = query.begin();
    int_vector<> query(2);
    query[0] = 1 , query[1] = 2;//query[2] = 31; 
    int_vector<>::iterator qit = query.begin();
    for(uint64_t pos=0;qit != query.end();++qit) {
      if(forward_search(cst, v, qit-query.begin(), *qit, pos) > 0) {
        
      } else {
        break;
      }
    }
    printf("After finding pattern...\n");

    // Find the word that save in the suffix node
    set< meta_node >::iterator mit;
    vector< QueryVersion > ans;
    while( 1 ) {
      int node_id = cst.id(v);
      printf("%d\n",node_id);
      mit = metadata.find( meta_node(node_id, -1) );
      if( mit != metadata.end() ) {

        SuffixTreeNode node_info;
        if( mit != metadata.end() && (mit->id) != -1 ) {
          // move the pointer to the node starting position
          int st_pos = mit->pos;
          ++mit;
          int ed_pos = mit->pos;
          fseek(p_node_info, st_pos, SEEK_SET);
          node_info.load(p_node_info);
          vector< QueryVersion > temp = node_info.query(start_time, end_time, topk);
          ans = merge_answer(ans, temp, topk);
        } else {
          // TODO (Bruce Kuo):
          //  Dummy node implementation
        }
      }
      if (v == cst.root() ) break;
      if (v == cst.parent(v)) break;
      v = cst.parent(v);
    }

    string answer("");
    for(vector< QueryVersion >::iterator it=ans.begin();it != ans.end();++it)
      answer += (it->toString()) + string("\n");
    fprintf(stdout, "%s\n", answer.c_str());
  }
  fclose(p_node_info);

  return 0;
}

