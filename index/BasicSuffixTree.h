/*
  Author: Bruce Kuo
  Date: 2013.03.03
  
  Description:
    This file implements our method in high level architecture.
*/

#ifndef __BASIC_SUFFIX_TREE_H__
#define __BASIC_SUFFIX_TREE_H__

#include "Document.h"
//#include "../lib/IntervalTree/interval_tree.h"
#include "base_util.h"

#include <algorithm>
#include <set>
#include <vector>

using std::vector;

class FreqNode 
{
// Help counting the frequency of a pattern in 
// the abstract tree leaves

public:
  // Constructor and Destructor
  FreqNode();
  FreqNode(int _docid, int _revid);
  ~FreqNode();

  // Operator of FreqNode
  bool operator < (const FreqNode &t) const;
  bool operator == (const FreqNode &t) const;

  // Method to access data
  int getFreq() const;
  void setFreq(int value);
  void increaseFreq();

  int getDocid() const;
  int getRevid() const;
  
  // serialize the data for output the freq node information
  vector<unsigned char> serialize();
  
private:
  int docid;
  int revid;
  int freq;
};

class SuffixTreeNode
{

// Basic definition of the data for our assumption

public:
  // Constructor and Destructor
  SuffixTreeNode(){};
  SuffixTreeNode(int _node_id): node_id(_node_id){};
  SuffixTreeNode(int _node_id, int _depth): node_id(_node_id), depth(_depth) {
    for(unsigned int i=0;i<freq.size();++i)
      freq[i].setFreq(0);
  }
  ~SuffixTreeNode();

  void addDoc(Document &doc);
  void queryInterval(int low, int high);
  void setFreqList(vector< FreqNode > &f){ freq = f; };
  void increaseFreq(int idx);
  
  void sortDocs();
  // get rid of hard code part to save space
  void buildDocs();

  int getFreq(int idx) const;
  int getNodeId() const;
  int getDepth() const;

  /*
    binary search the document that contains the document
    @param:
       x: the ending time of the query
    @return
       the starting position of the query  
   */
  int binary_search(long long x);

  // extract time interval top-k query
  string query(long long st_time, long long ed_time, long long top_k);

  /* 
    load node information from disk
    @param:
      fp: the pointer of the node in the file
    @return:
      the bytes of the node.
      if fail, return -1
   */
  int load(FILE *fp);
  

  // serialize the data for output to index
  vector<unsigned char> serialize();

  bool operator < (const SuffixTreeNode &t) const {
    return (depth < t.getDepth() || (depth == t.getDepth() && node_id > t.getNodeId()));
  }

private:
  int node_id;
  int depth;
  //int_vector<20> freq_list(65536) ;
  vector< FreqNode > freq;
  vector< Document > doc_list;
  //IntervalTree doc_list;
};


bool doc_cmp(const Document &a, const Document &b) {
  return a.GetLowPoint() < b.GetLowPoint();
}


// move the implementation here
FreqNode::FreqNode() {
  freq = 0;
}

FreqNode::FreqNode(int _docid, int _revid) {
  docid = _docid; revid = _revid;
}

FreqNode::~FreqNode(){}

bool FreqNode::operator < (const FreqNode &t) const {
  return (docid < t.getDocid() ||
          (docid == t.getDocid() && revid < t.getRevid()));
}

int FreqNode::getFreq() const {
  return freq;
}

int FreqNode::getDocid() const {
  return docid;
}

int FreqNode::getRevid() const {
  return revid;
}

void FreqNode::setFreq(int value) {
  freq = value;
}

void FreqNode::increaseFreq() {
  ++freq;
}

vector<unsigned char> FreqNode::serialize() {
  vector<unsigned char> output;
  vector<unsigned char> trans = intToBytes(docid);
  output.insert(output.end(), trans.begin(), trans.end());
  trans = intToBytes(revid);
  output.insert(output.end(), trans.begin(), trans.end());
  trans = intToBytes(freq);
  output.insert(output.end(), trans.begin(), trans.end());
  return output;
}


SuffixTreeNode::~SuffixTreeNode(){}

void SuffixTreeNode::addDoc(Document &doc) {
  // left todo
}

void SuffixTreeNode::increaseFreq(int idx) {
  freq[idx].increaseFreq();
}

void SuffixTreeNode:sortDocs() {
  std::sort(doc_list.begin(), doc_list.end(), doc_cmp);
}

void SuffixTreeNode::buildDocs() {
  // build up compact doc_list
  doc_list.clear();
  vector<int> doc_appear(freq_list.size() + 1);
  std::fill(doc_appear.begin(), doc_appear.end(), -1);
  for(int i=0;i<freq_list.size();++i) {
    if( freq_list[i].getFreq() == 0 ) continue;
    int x;
    if( doc_appear[ freq_list[i].getDocid() ] == -1 ) {
      doc_appear[i] = x = doc_list.size();
      doc_list.push_back(Document(freq_list[i].getDocid()));
    } else {
      x = doc_appear[freq_list[i].getDocid()];
    }
    doc_list[x].insertVersion(Version(freq_list[i].getRevid(), freq_list[i].getStartTime(), freq_list[i].getFreq()));
  }
  sortDocs();
  freq_list.clear();
  co_appear().clear();
  for(int i=0;i<doc_list.size();++i)
    doc_list[i].sortList();
}

vector<unsigned char> SuffixTreeNode::serialize() {
  vector<unsigned char> output;
  // save node id
  vector<unsigned char> trans = intToBytes(node_id);
  output.insert(output.end(), trans.begin(), trans.end());
  // save depth
  trans = intToBytes(depth);
  output.insert(output.end(), trans.begin(), trans.end());
  // save doc_list size
  int size = doc_list.size();
  trans = intToBytes(size);
  output.insert(output.end(), trans.begin(), trans.end());
  // save doc_list
  for(int i=0;i<size;++i) {
    trans = doc_list[i].serialize();
    output.insert(output.end(), trans.begin(), trans.end());
  }

  /*
  Fixed by Bruce Kuo: 2013.05.13
  replaced by doc_list serialize

    for(unsigned int i=0;i<freq.size();++i) {
      trans = freq[i].serialize();
      output.insert(output.end(), trans.begin(), trans.end());
    }
  */
  return output;
}

int SuffixTreeNode::load(FILE *fp) {
  int size = 0;
  node_id = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  depth = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  int d_size = doc_list.size();
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  doc_ilst[i].resize(d_size);
  for(int i=0;i<d_size;++i) {
    int move = doc_list[i].load(fp);
    fseek(fp, move, SEEK_CUR);
    size += move;
  }
  return size;
}

int SuffixTreeNode::binary_search(long long x) {
  int st = 0, ed = doc_list.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;
    if( doc_list[mid].GetLowPoint() <= x )
      st = mid + 1;
    else
      ed = mid;
  }
  return ed;
}

string SuffixTreeNode::query(long long st_time, long long ed_time, int top_k) {
  vector< QueryVersion > ans;
  //int st_pos = binary_search(start_time);
  //int ed_pos = binary_search(end_time);
  int st_pos = binary_search(start_time);

  for(int i=st_pos; i < doc_list[i].size();++i) {
    vector< QueryVersion > tmp = doc_list[i].query(start_time, end_time, top_k);
    vector< QueryVersion > v;
    vector< QueryVersion >::iterator pv = ans.begin();
    vector< QueryVersion >::iterator nv = tmp.begin();
    while( v.size() < top_k && pv != ans.end() && nv != tmp.end() ) {
      if( pv->getFreq() > nv->getFreq() )
        v.push_back(*pv);
      else
        v.push_back(*nv);
    }
    ans = v;
  }
  string answer = "";
  for(vector< QueryVersion >::iterator it=ans.begin();it != ans.end();++it)
    answer += it->toString();
  return answer;
}

int SuffixTreeNode::getNodeId() const {
  return node_id;
}

int SuffixTreeNode::getDepth() const {
  return depth;
}

int SuffixTreeNode::getFreq(int idx) const {
  return freq[idx].getFreq();
}

#endif
