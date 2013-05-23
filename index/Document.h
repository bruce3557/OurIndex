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
  Version(int _revid, int _start_time, int _freq): revid(_revid), start_time(_start_time), end_time(_start_time), freq(_freq){}
  Version(int _revid, int _start_time, int _end_time, int _freq);
  ~Version();

  int getID() const;
  int getStartTime() const;
  int getEndTime() const;
  int getFreq() const;

  vector<unsigned char> serialize();
  
  /*
    load version data from disk
    @param:
      fp: the starting pointer of the version
    @return:
      size of the version
      if fail, return -1
   */
  int load(FILE *fp);

private:
  int revid;
  int start_time;
  int end_time;
  int freq;
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

  vector<unsigned char> serialize();

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
  int start_time;
  int end_time;
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
  trans = intToBytes(start_time);
  output.insert(output.end(), trans.begin(), trans.end());
  // save frequency
  trans = intToBytes(freq);
  output.insert(output.end(), trans.begin(), trans.end());
  return output;
}

int load(FILE *fp) {
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
  trans = intToBytes(start_time);
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

int load(FILE *fp) {
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
