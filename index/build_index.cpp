/* 
 * Author: Bruce Kuo
 * Date: 2013.04.30
 * Description:
 *   building index for target data
 */


// SDSL headers
#include <sdsl/suffix_trees.hpp>
#include <sdsl/util.hpp>

#include "BasicSuffixTree.h"

// C headers
#include <cstdio>
#include <cstring>

// C++ headers
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

#include "BasicSuffixTree.h"
#include "Document.h"

using namespace sdsl;
using namespace std;

FILE *input;

//typedef cst_sada<> tCST;
typedef cst_sada<csa_bitcompressed<int_alphabet<> > > tCST;

int num_doc;
int DOCID[200000];
int REVID[200000];

int locate_doc(int_vector<> &REVID, int pos) {
  // locate which document contains this string
  int st = 0;
  int ed = REVID.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;

    if( REVID[mid] <= pos )
      st = mid + 1;
    else
      ed = mid;
  }
  return ed;
}

// compare function for sorting node list to output
bool vec_cmp(const SuffixTreeNode &a, const SuffixTreeNode &b) {
  return a.getNodeId() < b.getNodeId();
}

void CST_Traversal(tCST &cst) {
  int_vector<> docsign;
  unsigned char dollar_str[] = "$";
  
  locate(cst, dollar_str, dollar_str + 1, docsign);
  std::sort(docsign.begin(), docsign.end());

  int num_docs = docsign.size();
  int_vector<> docprev(num_docs + 1);
  for(int i = 0;i <= num_docs;++i)
    docprev[i] = -1;

  /*
   * TODO (Bruce Kuo):
   * I think we need to implement a better implementation for this part
   * Better is to avoid to allocate additional memory here.
   * (Now we use a queue to store the level-8 node. We should throw it 
   * if we can)
   */

  queue< SuffixTreeNode > SQ;
  for(tCST::const_iterator it = cst.begin(); it != cst.end(); ++it) {
    if( it.visit() == 1 ) {
      tCST::node_type v = *it;
      int depth = cst.depth(v);
    
      if( depth == 8 ) {
        // To traverse the nodes at this subtree
        int lid = cst.lb(v);
        int rid = cst.rb(v);
        vector< FreqNode > freq(num_docs + 1);
        for(int i = 0;i <= num_docs; ++i) {
          freq[i] = FreqNode(DOCID[i], REVID[i]);
        }
        for(int i = lid;i <= rid;++i) {
          int doc_pos = locate_doc(docsign, cst.csa[i]);
          freq[doc_pos].increaseFreq();
        }

        int node_id = cst.id(v);
        SQ.push( SuffixTreeNode(node_id, depth) );
      }
    }
  }

  set< SuffixTreeNode > STN;
  priority_queue< SuffixTreeNode > PQ;
  while( !SQ.empty() ) {
    SuffixTreeNode x = SQ.front();
    SQ.pop();
    for(int i=0;i<num_doc;++i) {
      if( x.getFreq(i) == 0 ) continue;
      if(docprev[i] != -1) {
        tCST::node_type v = cst.inv_id( x.getNodeId() );  
        tCST::node_type u = cst.inv_id( docprev[i] );
        tCST::node_type lca = cst.lca(u, v);

        int lca_id = cst.id(lca);
        int lca_depth = cst.depth(lca);
        //
        // Finish the HSV framework
        //
        // Temporary we use set to implement this framework to check whether
        // a suffix node is inside
        //
        set< SuffixTreeNode >::iterator it = STN.find( SuffixTreeNode(lca_id, lca_depth) );
        if( it != STN.end() ) {
          // Found SuffixTreeNode
          SuffixTreeNode tx = (*it);
          STN.erase(it);
          tx.increaseFreq(i);
          STN.insert(tx);
        } else {
          // Not Found
          SuffixTreeNode tx = SuffixTreeNode(lca_id, lca_depth);
          tx.increaseFreq(i);
          STN.insert(tx);
        }
      }
      docprev[i] = x.getNodeId();
    }
  }

  for(set< SuffixTreeNode >::iterator it=STN.begin(); it != STN.end() ; ++it)
    PQ.push(*it);
   
  /*
   * Iterate traverse each level's LCA until root.
   *
   * If PQ.size is equal to 1, that must be root node
   * So we don't need to handle this case
   */
  while( PQ.size() > 1 ) {
    set< SuffixTreeNode > MTN;
    for(int i=0;i<num_doc;++i)
      docprev[i] = -1;

    while( !PQ.empty() ) {
      SuffixTreeNode x = PQ.top();
      PQ.pop();

      for(int i=0;i<num_doc;++i) {
        if( x.getFreq(i) == 0 ) continue;
        if( docprev[i] != -1 ) {
          tCST::node_type v = cst.inv_id(x.getNodeId());
          tCST::node_type u = cst.inv_id(docprev[i]);
          tCST::node_type lca = cst.lca(u, v);

          int lca_id = cst.id(lca);
          int lca_depth = cst.depth(lca);
          set< SuffixTreeNode >::iterator it = MTN.find( SuffixTreeNode(lca_id, lca_depth) );
          if( it != MTN.end() ) {
            SuffixTreeNode tx = (*it);
            MTN.erase(it);
            tx.increaseFreq(i);
            MTN.insert(tx);
          } else {
            SuffixTreeNode tx = SuffixTreeNode(lca_id, lca_depth);
            tx.increaseFreq(i);
            MTN.insert(tx);
          }
        }
        docprev[i] = x.getNodeId();
      }
    }
 
    for(set< SuffixTreeNode >::iterator it=MTN.begin(); it != MTN.end() ; ++it) {
      PQ.push(*it);
      STN.insert(*it);
    }
    MTN.clear();
  }
  
  vector< SuffixTreeNode > vec;
  for(set< SuffixTreeNode >::iterator it=STN.begin();it != STN.end(); ++it)
    vec.push_back(*it);
  std::sort(vec.begin(), vec.end(), vec_cmp);
  STN.clear();

  // Save to index file
  int position = 0;
  FILE *metadata = fopen("metadata.idx", "w");
  FILE *node_idx = fopen("node_info.idx", "w");
  fprintf(metadata, "0");
  for(int i=0;i<vec.size();++i) {
    vector<unsigned char> x = vec[i].serialize();
    position += x.size();
    fprintf(metadata, " %d %d", vec[i].getNodeId(), position);
    //fwrite(position, 4, 1, metadata);
    fwrite((const unsigned char *)&x[0], 1, x.size(), node_idx);
  }
  fprintf(metadata, " -1");
  fclose(metadata);
  fclose(node_idx);
  store_to_file(cst, string("cst.idx"));
}

void PrintUsage(char *str) {
  printf("Usage: %s input output\n", str);
}

int main(int argc, char *argv[]) {
  if(argc < 3) {
    PrintUsage(argv[0]);
    return 0;
  }

  input = fopen(argv[1], "r");
  
  tCST cst;
  construct(cst, argv[1], 'd');
  
  CST_Traversal(cst);

  return 0;
}

