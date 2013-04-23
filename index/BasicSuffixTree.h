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
  int getFreq(int idx) const;
  int getNodeId() const;
  int getDepth() const;

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
  IntervalTree doc_list;
};



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

vector<unsigned char> SuffixTreeNode::serialize() {
  vector<unsigned char> output;
  vector<unsigned char> trans = intToBytes(node_id);
  output.insert(output.end(), trans.begin(), trans.end());
  trans = intToBytes(depth);
  output.insert(output.end(), trans.begin(), trans.end());
  for(unsigned int i=0;i<freq.size();++i) {
    trans = freq[i].serialize();
    output.insert(output.end(), trans.begin(), trans.end());
  }
  return output;
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
