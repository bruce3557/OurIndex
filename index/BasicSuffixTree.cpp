/*
  Author: Bruce Kuo
  Date: 2013.04.09

  Description:
    Implement the suffix tree related class and function 
*/

#include "BasicSuffixTree.h"


// Implement the method in FreqNode class 
FreqNode::FreqNode() {
  freq = 0;
}

FreqNode::FreqNode(int _docid, int _revid) {
  docid = _docid; revid = _revid;
}

FreqNode::~FreqNode(){}

bool FreqNode::operator < (const FreqNode &t) const {
  return (docid < t.getDocid() || (docid == t.getDocid() && revid < t.getRevid()));
}

bool FreqNode::operator == (const FreqNode &t) const {
  return (docid == t.docid && revid == t.revid);
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

vector<unsigned char> FreqNode::searialize() {
  vector<unsigned char> output;
  vector<unsigned char> trans = intToBytes(docid);
  output.insert(output.end(), trans.begin(), trans.end());
  trans = intToBytes(revid);
  output.insert(output.end(), trans.begin(), trans.end());
  trans = intToBytes(freq);
  output.insert(output.end(), trans.begin(), trans.end());
  return output;
}


// Implement the method in SuffixTreeNode class
SuffixTreeNode::~SuffixTreeNode(){}

void SuffixTreeNode::addDoc(Document &doc) {
  //doc_list.Insert(doc);
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
    trans = freq[i].searialize();
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

