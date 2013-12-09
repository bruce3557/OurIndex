/*
 * Author: Bruce Kuo
 * Date: 2013.07.06
 *
 * Description:
 *  This is for R-tree design suffix tree node
 */

#ifndef __R_SUFFIX_TREE_H__
#define __R_SUFFIX_TREE_H__

#include "../lib/RTree/RTree.h"
#include "base_util.h"
#include "BasicSuffixTree.h"

#include <stdio.h>
#include <unistd.h>

bool RTreeSearchCallback(int id, void *arg);

struct Rect {
  /*
   * the R-Tree element of a document
   */
  Rect(){}
  Rect(int min_time, int max_time, int _score, int _org_id) {
    min[0] = min_time;
    max[0] = max_time;
    score = score;
    min[1] = max[1] = score;
    org_id = _org_id;
  }
  Rect(int min_time, int max_time, int _score, int _doc_id, int _rev_id) {
    min[0] = min_time;
    max[0] = max_time;
    score = _score;
    min[1] = max[1] = score;
    doc_id = _doc_id;
    rev_id = _rev_id;
  }

  // 0 is time
  // 1 is score
  int org_id;
  int doc_id;
  int rev_id;
  int score;
	int min[2];
  int max[2];
};

class RSuffixTreeNode {
  // R-Tree Based suffix tree implementation
  
  public:
    RSuffixTreeNode(){};
    RSuffixTreeNode(int _node_id): node_id(_node_id){};
    RSuffixTreeNode(int _node_id, int _depth): node_id(_node_id), depth(_depth) {
      for(unsigned int i=0;i<freq_list.size();++i)
        freq_list[i].setFreq(0);
    };
    RSuffixTreeNode(int _node_id, int _depth, int _num_doc): node_id(_node_id), depth(_depth) {
      freq_list.resize(_num_doc);
      for(unsigned int i=0;i<_num_doc;++i)
        freq_list[i].setFreq(0);
    }
    RSuffixTreeNode(int _node_id, int _depth, vector< FreqNode > &_freq): node_id(_node_id), depth(_depth) {
      for(int i=0;i < _freq.size();++i)
        freq_list.push_back(_freq[i]);
    }
    ~RSuffixTreeNode(){};

    virtual void addDoc(Rect &rt);
    virtual void setFreqList(vector< FreqNode > &f) {
      freq_list = f;
    }
    virtual void increaseFreq(int idx);

    // sort the document list
    virtual void sortDocs();
    virtual void buildDocs();

    /*
     * get the frequency of the index i
     * @param:
     *   idx: the index
     * @return:
     *   the value of the document idx
     */
    virtual int getFreq(int idx) const;
    virtual int getNodeId() const;
    virtual int getDepth() const;

    /*
     * binary search the document that contains the document
     * @param:
     *   x: the ending time of the query
     * @return:
     *   the starting position of the query
     */
    // We aren't sure whether the implementation is
    // need in this structure, preserved
    //virtual int binary_serach(long long x);

    /* TODO (Bruce Kuo):
     *   finish the query function
     */
    //virtual vector< Rect > query(long long st_time, long long ed_time, int top_k);
    virtual vector< int > query(long long st_time, long long ed_time, int threshold);

    virtual int load(FILE *fp);
    /*
     * serialize the data for output to index
     * @return:
     *   the byte stream of the node
     */
    virtual vector<unsigned char> serialize();

    bool operator < (const RSuffixTreeNode &t) const {
      return (depth < t.getDepth() || (depth == t.getDepth() && node_id > t.getNodeId()));
    }

  private:
    int node_id;
    int depth;
    vector< FreqNode > freq_list;
    RTree<int, int, 2, float> rtree; 
};
#endif
