/*
 * Author: Bruce Kuo
 * Date: 2013.07.12
 * Description:
 *   building r-tree based index for target data
 */

// SDSL headers
#include <sdsl/suffix_trees.hpp>
#include <sdsl/util.hpp>

#include "RSuffixTree.h"
#include "Document.h"

// C headers
#include <cstdio>
#include <cstring>

// C++ headers
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

using namespace sdsl;
using namespace std;

FILE *input;

typedef cst_sada<> tCST;

int num_doc;
int DOCID[200000];
int REVID[200000];

bool
vec_cmp(const RSuffixTreeNode &a, const RSuffixTreeNode &b) {
  return (a.getNodeId() < b.getNodeId());
}

void
build_docids(char *path, int num_docs) {
  fprintf(stderr, "reading %s...\n", path);
  FILE *fp = fopen(path, "r");
  if(fp == NULL)  return;
  int i = 0;
  fprintf(stderr, "start reading!\n");
  while( fscanf(fp, "%d%d", &DOCID[i], &REVID[i]) != EOF ) {
    ++i;
  }
}

int 
locate_doc(int_vector<> &REVID, int pos) {
  int st = 0, ed = REVID.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;

    if( REVID[mid] <= pos )
      st = mid + 1;
    else
      ed = mid;
  }
  return ed;
}

void 
CST_Traversal(tCST &cst) {
  fprintf(stderr, "Start building...");
  int_vector<> docsign;
  string dollar_str("$");


  set< DocObject > docObj; 
  locate(cst, dollar_str.begin(), dollar_str.end(), docsign);
  //std::sort(docsign.begin(), docsign,end());
  
  int num_docs = docsign.size();
  int_vector<> docprev(num_docs + 1);
  for(int i = 0;i <= num_docs;++i)
    docprev[i] = -1;
  build_docids("/home/bruce3557/Data/Result_2.8G/total_id", num_docs);

  //build up doc_obj
  buildDocSet("/home/bruce3557/Data/Result_2.8G/total_id", docObj);


  printf("Starting build HSV + Rtree framework...\n");
  queue< RSuffixTreeNode > SQ;
  for(tCST::const_iterator it = cst.begin(); it != cst.end(); ++it) {
    if( it.visit() == 1) {
      tCST::node_type v = *it;
      int depth = cst.depth(v);

      if(depth == 8) {
        // Traverse the nodes at this subtree
        int lid = cst.lb(v);
        int rid = cst.rb(v);
        vector< FreqNode > freq(num_docs + 1);
        for(int i = 0;i < num_docs; ++i)
          freq[i] = FreqNode(DOCID[i], REVID[i]);

        for(int i = lid; i <= rid; ++i) {
          int doc_pos = locate_doc(docsign, cst.csa[i]);
          freq[doc_pos].increaseFreq();
        }

        int node_id = cst.id(v);
        SQ.push( RSuffixTreeNode(node_id, depth, freq) );
      }
    }
  }

  printf("Go, %d\n", SQ.size());
  fflush(stdout);
  num_doc = num_docs;
  set< RSuffixTreeNode > STN;
  priority_queue< RSuffixTreeNode > PQ;
  int c_c = 0;
  while( !SQ.empty() ) {
    RSuffixTreeNode x = SQ.front();
    SQ.pop();
    for(int i = 0;i < num_doc;++i) {
      if( x.getFreq(i) == 0 ) continue;
      if( docprev[i] != -1 ) {
        tCST::node_type v = cst.inv_id( x.getNodeId() );
        tCST::node_type u = cst.inv_id( docprev[i] );
        tCST::node_type lca = cst.lca(u, v);
        int lca_id = cst.id(lca);
        int lca_depth = cst.depth(lca);

        // Finish HSV + RTree framework
        set< RSuffixTreeNode >::iterator it = STN.find( RSuffixTreeNode(lca_id, lca_depth) );
        if( it != STN.end() ) {
          RSuffixTreeNode tx = (*it);
          STN.erase(*it);
          tx.increaseFreq(i);
          STN.insert(tx);
        } else {
          RSuffixTreeNode tx = RSuffixTreeNode(lca_id, lca_depth, num_doc);
          tx.increaseFreq(i);
          STN.insert(tx);
        }
      }
      docprev[i] = x.getNodeId();
    }
  }

  printf("Build up structure..\n");
  printf("STN size = %d\n", STN.size());
  for(set< RSuffixTreeNode >::iterator it=STN.begin(); it != STN.end(); ++it) {
    PQ.push(*it);
  }

  /*
   * Iterate traverse each level's LC until root.
   *
   * If PQ.size is equal to 1, that must be root node
   * So we don't need to handle this case
   */
  while( PQ.size() > 1 ) {
    set< RSuffixTreeNode > MTN;
    for(int i=0;i < num_doc;++i)
      docprev[i] = -1;
    
    while( !PQ.empty() ) {
      RSuffixTreeNode x = PQ.top();
      PQ.pop();

      for(int i = 0;i < num_doc;++i) {
        if( x.getFreq(i) == 0 ) continue;
        if( docprev[i] != -1 ) {
          tCST::node_type v = cst.inv_id(x.getNodeId());
          tCST::node_type u = cst.inv_id(docprev[i]);
          tCST::node_type lca = cst.lca(u, v);
          int lca_id = cst.id(lca);
          int lca_depth = cst.depth(lca);
          set< RSuffixTreeNode >::iterator it = MTN.find( RSuffixTreeNode(lca_id, lca_depth) );
          if( it != MTN.end() ) {
            RSuffixTreeNode tx = (*it);
            MTN.erase(it);
            tx.increaseFreq(i);
            MTN.insert(tx);
          } else {
            RSuffixTreeNode tx = RSuffixTreeNode(lca_id, lca_depth, num_doc);
            tx.increaseFreq(i);
            MTN.insert(tx);
          }
        }
        docprev[i] = x.getNodeId();
      }
    }
    for(set< RSuffixTreeNode >::iterator it=MTN.begin(); it != MTN.end(); ++it) {
      PQ.push(*it);
      STN.insert(*it);
    }
    MTN.clear();
  }
  
  printf("STN size = %d\n", STN.size());
  vector< RSuffixTreeNode > vec;
  for(set< RSuffixTreeNode >::iterator it = STN.begin(); it != STN.end(); ++it)
      vec.push_back(*it);
  STN.clear();
  std::sort(vec.begin(), vec.end(), vec_cmp);

  printf("Start write to output file\n");
  int position = 0;
  FILE *metadata = fopen("metadata.idx", "w");
  FILE *node_idx = fopen("node_info.idx", "w");
  fprintf(metadata, "0");
  for(int i=0;i<vec.size();++i) {
    vector<unsigned char> x = vec[i].serialize();
    position += x.size();
    fprintf(metadata, " %d %d", vec[i].getNodeId(), position);
    fwrite((const unsigned char *)&x[0], 1, x.size(), node_idx);
  }
  fprintf(metadata, " -1");
  fclose(metadata);
  fclose(node_idx);
  store_to_file(cst, string("cst.idx"));
}

void 
PrintUsage(char *str) {
  printf("Usage: %s input output\n", str);
}

int 
main(int argc, char *argv[]) {
  if(argc < 3) {
    PrintUsage(argv[0]);
    return 0;
  }

  input = fopen(argv[1], "r");

  tCST cst;
  construct(cst, argv[1], 1);

  CST_Traversal(cst);

  return 0;
}
