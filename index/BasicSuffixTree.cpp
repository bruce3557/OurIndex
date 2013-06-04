#include "BasicSuffixTree.h"

bool doc_cmp(const Document &a, const Document &b) {
  return a.GetLowPoint() < b.GetLowPoint();
}

vector< QueryVersion > 
merge_answer(vector< QueryVersion > &a, vector< QueryVersion > &b, int top_k) {
  vector< QueryVersion > v;
  vector< QueryVersion >::iterator pv = a.begin();
  vector< QueryVersion >::iterator nv = b.begin();
  while( v.size() < top_k && pv != a.end() && nv != b.end() ) {
    if( pv->getFreq() > nv->getFreq() )
      v.push_back(*pv);
    else
      v.push_back(*nv);
  }
  return v;
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
  freq_list[idx].increaseFreq();
}

void SuffixTreeNode::sortDocs() {
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
  doc_appear.clear();
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
  doc_list.resize(d_size);
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

vector< QueryVersion > SuffixTreeNode::query(long long st_time, long long ed_time, int top_k) {
  vector< QueryVersion > ans;
  int st_pos = binary_search(st_time);

  for(int i=st_pos; i < doc_list.size() && doc_list[i].getStartTime() < ed_time; ++i) {
    vector< QueryVersion > tmp = doc_list[i].query(st_time, ed_time, top_k);
    ans = merge_answer(ans, tmp, top_k);
  }
  return ans;
}

int SuffixTreeNode::getNodeId() const {
  return node_id;
}

int SuffixTreeNode::getDepth() const {
  return depth;
}

int SuffixTreeNode::getFreq(int idx) const {
  return freq_list[idx].getFreq();
}

