/*
  Author: Bruce Kuo
  Date: 2013.02.13
*/

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include "../lib/IntervalTree/interval_tree.h"

//#include <sdsl/int_vector.hpp>
//#include <sdsl/util.hpp>

#include <vector>
#include <algorithm>
using std::vector;

class Version
{
// Definition of version
// TODO(Bruce Kuo): Discuss that whether end_time need to exists

public:
  Version();
  Version(int _vid, int _start_time, int _end_time, int _freq);
  ~Version();

  int getID() const;
  int getStartTime() const;
  int getEndTime() const;
  int getFreq() const;
  
private:
  int revid;
  int start_time;
  int end_time;
  int freq;
};


class Document : public Interval 
{

// Basic document class
// This must inherit interval class because document will be
// treated as a node in interval tree

public:
  Document();
  Document(int _docid);
  ~Document();
  void insertVersion(const Version &ver);
  int searchVer(const Version &ver);
  int searchVer(const int query_time);
  void sortList();

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
                 int _end_time, int _freq) {
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
  
}

#endif
