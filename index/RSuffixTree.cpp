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
  int size = 0;
  node_id = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  depth = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  char fpname[200];
  char curDir[200];

  if( !getcwd(curDir, sizeof(curDir)) ){
    fprintf(stderr, "ERROR when getting current director\n");
    return -1;
  }
  sprintf(fpname, "%s\\Rtree\\%d.idx", curDir, node_id);
  rtree.Load(fpname);

  return size;
}

vector<unsigned char> RSuffixTreeNode::serialize() {
  vector<unsigned char> output;
  // save node id
  vector<unsigned char> trans = intToBytes(node_id);
  output.insert(output.end(), trans.begin(), trans.end());
  // save depth
  trans = intToBytes(depth);
  output.insert(output.end(), trans.begin(), trans.end());
  // save file
  char fpname[200];
  char curDir[200];
  if( !getcwd(curDir, sizeof(curDir)) ) {
    fprintf(stderr, "ERROR when getting current diretory\n");
  }
  sprintf(fpname, "%s\\Rtree\\%d.idx", curDir, node_id);
  rtree.Save(fpname);

  return output;
}

bool RTreeSearchCallback(int id, void *arg) {
  vector< int > &answer = *((vector< Rect >*)(arg[0]));
  answer.push_back(id);
  return true;
}

vector< int > RSuffixTreeNode::query(long long st_time, long long ed_time, int threshold) {
  vector< Rect > answer;
  Rect searchRect(st_time, ed_time, threshold, -1);
  int nhits = tree.Search(searchRect.min, searchRect.max, RTreeSearchCallback, (void*)(&answer));
  fprintf(stderr, "%d hits~~\n", nhits);
  return answer;
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
