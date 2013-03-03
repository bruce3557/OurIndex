/*
  Author: Bruce Kuo
  Date: 2013.02.13

  Implementation of defined class
*/

#include "Document.h"
#include <algorithm>

Version::Version(){}
Version::~Version(){}

Version::Version(int _revid, int _start_time, 
                 int _end_time, int _freq) {
  revid = _revid;
  start_time = _start_time;
  end_time = _end_time;
  freq = _freq;
}

int Version::getID() {
  return revid;
}

int Version::getStartTime() {
  return start_time;
}

int Version::getEndTime() {
  return end_time;
}

int Version:getFreq() {
  return freq;
}


Document::Document(){}
Document::~Document(){}

Document::Document(int _docid) {
  docid = _docid;
  ver_list.clear();
}

bool ver_cmp(const Version &a, const Version &b) {
  return (a.time < b.time);
}

int searchVer(const Version &ver) {
  int st = 0;
  int ed = ver_list.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;
    if( ver_list[mid].start_time < ver.start_time )
      st = mid + 1;
    else
      ed = mid;
  }
  return ed;
}

int searchVer(const int query_time) {
  int st = 0;
  int ed = ver_list.size();
  while(st < ed) {
    int mid = (st + ed) >> 1;
    if( ver_list[mid].start_time < query_time )
      st = mid + 1;
    else
      ed = mid;
  }
  return ed;
}

void insertVersion(const Version &ver) {
  int pos = searchVer(ver);
  ver_list.insert(ver_list.begin() + pos, ver);
  start_time = ver.start_time < start_time ? ver.start_time : start_time;
  end_time = ver.start_time > end_time ? ver.start_time : end_time;
}

void Document::sortList() {
  std::sort(ver_list.begin(), ver_list.end(), ver_cmp);
}

int Document::GetLowPoint() const = 0 {
  return start_time;
}

int Document::GetHighPoint() const = 0 {
  return end_time;
}

void Document::Print() const {

}
