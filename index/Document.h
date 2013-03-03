/*
  Author: Bruce Kuo
  Date: 2013.02.13
*/

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include "../lib/interval_tree.h"

#include <vector>
using std::vector;

class Version
{
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
  
private:
  int revid;
  int start_time;
  int end_time;
  int freq;
};


class Document : Interval 
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
  int GetLowPoint() const = 0;
  int GetHighPoint() const = 0;
  vovid Print() const;

private:
  int docid;
  int start_time;
  int end_time;
  vector< Version > ver_list;

};

#endif
