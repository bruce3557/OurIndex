/*
  Author: Bruce Kuo
  Date: 2013.02.13
*/

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include "../lib/interval_tree.h"

#include <vector>
using std::vector;

class Version : Interval {
// Definition of version
// TODO(Bruce Kuo): Discuss that whether end_time need to exists

public:
  Version();
  Version(int _vid, int _start_time, int _end_time, int _freq);
  ~Version();

  int getID();
  int getStartTime();
  int getEndTime();
  int getFreq();
  
  // implementation from interval class
  int GetLowPoint() const = 0;
  int GetHighPoint() const = 0;
  void Print() const;


private:
  int revid;
  int start_time;
  int end_time;
  int freq;
};

class Document {

public:
  Document();
  Document(int _docid);
  ~Document();
  void insertVersion(const Version &ver);
  int searchVer(const Version &ver);
  int searchVer(const int query_time);
  void sortList();

private:
  int docid;
  int start_time;
  int end_time;
  vector< Version > ver_list;

};

#endif
