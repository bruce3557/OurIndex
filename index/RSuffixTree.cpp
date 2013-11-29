#include "RSuffixTree.h"

#include <algorithm>

void RSuffixTreeNode::addDoc(Rect &rt) {
  // left empty
}

void RSuffixTreeNode::increaseFreq(int idx) {
  freq_list[idx].increaseFreq();
}

void RSuffixTreeNode::sortDocs() {
  // TODO (Bruce Kuo):
  //   left to discussion
  //std::sort(freq_list.begin(), freq_list.end(), doc_cmp); 
}

int RSuffixTreeNode::getFreq(int idx) const {
  freq_list[idx].getFreq();
}

int RSuffixTreeNode::getNodeId() const {
  return node_id;
}

int RSuffixTreeNode::getDepth() const {
  return depth;
}

int RSuffixTreeNode::load(FILE *fp) {

}

vector<unsigned char> RSuffixTreeNode::serialize() {

}

void RSuffixTreeNode::query(long long st_time, long long ed_time, int threshold) {
  
}

void RSuffixTreeNode::buildDocs() {
  rtree.RemoveAll();
  vector<int> doc_appear(freq_list.size() + 1);
  std::fill(doc_appear.begin(), doc_appear.end(), -1);
  int freq = -1;
  long long st_time, ed_time, docid;
  long long st_rev, ed_rev;
  long long st_doc;

  for(int i=0;i<freq_list.size();++i) {
    int docid, revid;
    docid = freq_list[i].getDocid();
    revid = freq_list[i].getRevid();
    set< DocObject >::iterator it = docObj.find(DocObject(docid, revid, 0));
    
    //if(freq_list[i].getFreq() != freq && docid != st_doc) {
      set< DocObject >::iterator it = docObj.find(DocObject(docid, revid, 0));
      if(freq != -1) {
        Rect rect(it->st_id, it->ed_id, freq, docid, revid);
        rtree.Insert(rect.min, rect.max, docid);
      }
      st_doc = docid;
      st_rev = revid;
      st_time = it->start_time;
      freq = freq_list[i].getFreq();
    //}
    
    ed_time = it->start_time;
    ed_rev = revid;
  }
}
