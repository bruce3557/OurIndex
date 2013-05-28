/*
  Author: Bruce Kuo
  Date: 2013.02.13

  Description:
    Document, Version definition
*/

/*
  TODO (Bruce Kuo):
    It may consider to use sdsl int_vector to compress the size.
    The difficulty is to serialize the data in the same file.
*/

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

//#include "../lib/IntervalTree/interval_tree.h"

//#include <sdsl/int_vector.hpp>
//#include <sdsl/util.hpp>

#include "base_util.h"

#include <vector>
#include <algorithm>
using std::vector;

class Version
{
// Definition of version
// TODO(Bruce Kuo): Discuss that whether end_time need to exists

public:
  Version();
  Version(int _revid, long long _start_time, int _freq): revid(_revid), start_time(_start_time), end_time(_start_time), freq(_freq){}
  Version(int _revid, long long _start_time, long long _end_time, int _freq);
  Version(Version &ver) {
    revid = ver.getID();
    start_time = ver.getStartTime();
    end_time = ver.getEndTime();
    freq = ver.getFreq();
  }
  ~Version();

  virtual int getID() const;
  virtual int getStartTime() const;
  virtual int getEndTime() const;
  virtual int getFreq() const;

  virtual vector<unsigned char> serialize();
  
  /*
    load version data from disk
    @param:
      fp: the starting pointer of the version
    @return:
      size of the version
      if fail, return -1
   */
  virtual int load(FILE *fp);

private:
  int revid;
  long long start_time;
  long long end_time;
  int freq;
};

class QueryVersion: public Version {
  QueryVersion(){};
  ~QueryVersion(){};
  QueryVersion(int _docid, Version &ver): Version(ver), doc_id(_doc_id) {}

  virtual string toString();

  virtual vector<unsigned char> serialize() {
    vector<unsigned char> output = intToBytes(docid);
    vector<unsigned char> prev = Version::serialize();
    output.insert(output.end(), prev.begin(), prev.end());
    return output;
  }

  private:
    int docid;
};

class Document 
{

// Basic document class
// This must inherit interval class because document will be
// treated as a node in interval tree
//
// Now we are based on the observation that every doc will be
// survived util now
// We can treat that each doc as a line and we just save the 
// start time

public:
  Document();
  Document(int _docid);
  ~Document();
  void insertVersion(const Version &ver);
  int searchVer(const Version &ver);
  int searchVer(const int query_time);
  void sortList();

  /*
    find the version that after the time point x
    @param:
      x: the time point
    @return:
      the first version position after x
   */
  int binary_search(long long x);

  /*
    transform data into bytes stream
    @return:
      bytes stream
   */
  vector<unsigned char> serialize();
  virtual string toString();
  
  /*
    query the top-k query in the document, 
    sorting order is by decreasing order
    @param:
      st_time: starting time of the query
      ed_time: ending time of the query
      top_k: the number of top-k query
    @return:
      the top-k version in this query
   */
  vector< QueryVer > query(long long st_time, long long ed_time, int top_k);

  /*
    load the document information from disk
    @param:
      fp: the pointer of the document in the file
    @return:
      the bytes of the document.
      if fail, return -1
   */
  int load(FILE *fp);

  // Inherit from basic interval class
  // These function need to be implemented for interval tree use
  virtual int GetLowPoint() const ;
  virtual int GetHighPoint() const ;
  virtual void Print() const;

private:
  int docid;
  long long start_time;
  long long end_time;
  vector< Version > ver_list;

};


// Move implementation here
Version::Version(){}
Version::~Version(){}

Version::Version(int _revid, int _start_time,
                 int start_end_time, int _freq) {
  revid = _revid;
  start_time = _start_time;
  end_time = _end_time;
  freq = _freq;
}

int Version::getID() const {
  return revid;
}

int Version::getStartTime() const {
  return start_time;
}

int Version::getEndTime() const {
  return end_time;
}

int Version::getFreq() const {
  return freq;
}

vector<unsigned char> Version::serialize() {
  vector<unsigned char> output;
  // save rev id
  vector<unsigned char> trans = intToBytes(rev_id);
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
  char x[20];
  string output("Rev ID: ");
  output += string( itoa(revid) );
  sprintf(x, "\nStarting time: %lld\n Freq: ", start_time);
  output += string(x);
  output += string( itoa(freq) ) + string("\n");
}

int Version::load(FILE *fp) {
  int size = 0;
  rev_id = bytesToInt(fp);
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

string QueryVer::toString() {
  string output("Doc ID: ");
  output += string(itoa(docid)) + string("\n");
  output += Version::toString();
  return output;
}

Document::Document() {}
Document::~Document() {}

Document::Document(int _docid) {
  docid = _docid;
  ver_list.clear();
}

bool ver_cmp(const Version &a, const Version &b) {
  return (a.getStartTime() < b.getStartTime());
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

void Document::Print() const {
  // TODO (Bruce Kuo):
  //  to present the data in a good manner
}

vector<unsigned char> Document::serialize() {
  vector<unsigned char> output;

  // save doc id
  vector<unsigned char> trans = intToBytes(doc_id);
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

vector< QueryVer > Document::query(long long st_time, long long ed_time, int top_k) {
  vector< QueryVer > ans;
  int st_pos = binary_search(st_time);
  int ed_pos = binary_search(ed_time);
  if( ver_list[st_pos-1].getStartTime() >= st_time )  --st_pos;
  for(int i=ed_pos-1;ans.size() < top_k && i >= st_pos;--i)
    ans.push_back( QueryVer(docid, ver_list[i]) );
  return ans;
}

string Document::toString() {
  string output("Doc ID: ");
  output += string( itoa(docid) ) + string("\n");
  for(vector< Version >::iterator it=ver_list.begin();it != ver_list.end();++it)
    output += string("--------------\n") + it->toString();
  return output;
}

int Document::load(FILE *fp) {
  int size = 0;
  doc_id = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  start_time = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  int v_size = bytesToInt(fp);
  fseek(fp, 4, SEEK_CUR);
  size += 4;
  ver_list[i].resize(v_size);
  for(int i=0;i<v_size;++i) {
    int move = ver_list[i].load(fp);
    fseek(fp, move, SEEK_CUR);
    size += move;
  }
  return size;
}

#endif
