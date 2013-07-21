#include "Document.h"

// Move implementation here

Version::Version(int _revid, long long _start_time,
                 long long _end_time, int _freq) {
  revid = _revid;
  start_time = _start_time;
  end_time = _end_time;
  freq = _freq;
}

int Version::getID() const {
  return revid;
}

long long Version::getStartTime() const {
  return start_time;
}

long long Version::getEndTime() const {
  return end_time;
}

int Version::getFreq() const {
  return freq;
}

vector<unsigned char> Version::serialize() {
  vector<unsigned char> output;
  // save rev id
  vector<unsigned char> trans = intToBytes(revid);
  output.insert(output.end(), trans.begin(), trans.end());
  // save start_time
  trans = longlongToBytes(start_time);
  output.insert(output.end(), trans.begin(), trans.end());
  // save frequency
  trans = intToBytes(freq);
  output.insert(output.end(), trans.begin(), trans.end());
  return output;
}

string Version::toString() {
  char x[330];
  memset(x, 0, sizeof(x));
  string output("Rev ID: ");
  sprintf(x, "%d", revid);
  output += string( x );
  memset(x, 0, sizeof(x));
  sprintf(x, "\nStarting time: %lld\n Freq: ", start_time);
  output += string(x);
  sprintf(x, "%d\0", freq);
  output += string( x ) + string("\n");
}

int Version::load(FILE *fp) {
  int size = 0;
  revid = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  start_time = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  freq = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  return size;
}

string QueryVersion::toString() {
  char buf[33];
  memset(buf, 0, sizeof(buf));
  string output("Doc ID: ");
  sprintf(buf, "%d", docid);
  output += string(buf) + string("\n");
  output += Version::toString();
  return output;
}

bool ver_cmp(const Version &a, const Version &b) {
  return (a.getStartTime() < b.getStartTime());
}

Document::Document(){
}

Document::~Document(){
}

Document::Document(const Document &doc) {
  docid = doc.getID();
  start_time = doc.GetLowPoint();
  ver_list = doc.getVerList();
}

Document::Document(int _docid) {
  docid = _docid;
  ver_list.clear();
}

long long Document::getStartTime() {
  return start_time;
}

void Document::sortList() {
  std::sort(ver_list.begin(), ver_list.end(), ver_cmp);
}

int Document::GetLowPoint() const {
  return start_time;
}

int Document::GetHighPoint() const {
  return end_time;
}

void Document::Print() {
  string output = toString();
  printf("%s\n", output.c_str());
}

vector<unsigned char> Document::serialize() {
  vector<unsigned char> output;

  // save doc id
  vector<unsigned char> trans = intToBytes(docid);
  output.insert(output.end(), trans.begin(), trans.end());
  // save start time
  trans = longlongToBytes(start_time);
  output.insert(output.end(), trans.begin(), trans.end());
  // save version vector size
  int size = ver_list.size();
  trans = intToBytes(size);
  output.insert(output.end(), trans.begin(), trans.end());
  // save version vector
  for(int i=0;i<size;++i) {
    trans = ver_list[i].serialize();
    output.insert(output.end(), trans.begin(), trans.end());
  }
  return output;
}

int Document::binary_search(long long x) {
  int st = 0, ed = ver_list.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;
    if( ver_list[mid].getStartTime() <= x)  st = mid + 1;
    else  ed = mid;
  }
  return ed;
}

vector< QueryVersion > Document::query(long long st_time, long long ed_time, int top_k) {
  vector< QueryVersion > ans;
  int st_pos = binary_search(st_time);
  int ed_pos = binary_search(ed_time);
  if( ver_list[st_pos-1].getStartTime() >= st_time )  --st_pos;
  for(int i=ed_pos-1;ans.size() < top_k && i >= st_pos;--i)
    ans.push_back( QueryVersion(docid, ver_list[i]) );
  return ans;
}

string Document::toString() {
  char buf[33];
  memset(buf, 0, sizeof(buf));
  string output("Doc ID: ");
  sprintf(buf, "%d", docid);
  output += string( buf ) + string("\n");
  for(vector< Version >::iterator it=ver_list.begin();it != ver_list.end();++it)
    output += string("--------------\n") + it->toString();
  return output;
}

int Document::load(FILE *fp) {
  int size = 0;
  docid = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  start_time = bytesToLonglong(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  int v_size = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  ver_list.resize(v_size);
  for(int i=0;i<v_size;++i) {
    int move = ver_list[i].load(fp);
    fseek(fp, move, SEEK_CUR);
    size += move;
  }
  return size;
}

void buildDocSet(char *filename, set< DocObject > &docObj, int &doc_count) {
  docObj.clear();
  FILE *fp = fopen(filename, "r");
  int docid, revid;
  char time_str[100];
  doc_count = 1;
  while( fscanf(fp, "%d%d%s", &docid, &revid, time_str) != EOF ) {
    long long ot_time = timeToLonglong(time_str);
    docObj.insert(DocObject(doc_count, docid, revid, ot_time));
    ++doc_count;
  }
  fclose(fp);
}
