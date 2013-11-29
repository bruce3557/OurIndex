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

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <set>
#include <vector>
#include <string>
#include <algorithm>

using std::vector;
using std::string;
using std::sort;
using std::set;

#include "base_util.h"

class Version {
// Definition of version
// TODO(Bruce Kuo): Discuss that whether end_time need to exists

public:
  Version(){};
  Version(int _revid, long long _start_time, int _freq): revid(_revid), start_time(_start_time), end_time(_start_time), freq(_freq){}
  Version(int _revid, long long _start_time, long long _end_time, int _freq);
  Version(Version &ver) {
    revid = ver.getID();
    start_time = ver.getStartTime();
    end_time = ver.getEndTime();
    freq = ver.getFreq();
  }
  Version(const Version &ver): revid(ver.getID()), start_time(ver.getStartTime()), end_time(ver.getEndTime()), freq(ver.getFreq()){}
  ~Version(){};

  virtual int getID() const;
  virtual long long getStartTime() const;
  virtual long long getEndTime() const;
  virtual int getFreq() const;

  virtual vector<unsigned char> serialize();
  virtual string toString();

  /*
    load version data from disk
    @param:
      fp: the starting pointer of the version
    @return:
      size of the version
      if fail, return -1
   */
  virtual int load(FILE *fp);

protected:
  int revid;
  long long start_time;
  long long end_time;
  int freq;
};

class QueryVersion: public Version {
	public:
	  QueryVersion(){};
		~QueryVersion(){};
		QueryVersion(const QueryVersion &ver): 
			Version::Version(ver.getID(), ver.getStartTime(), ver.getFreq()), docid(ver.getID()){}
	  QueryVersion(int _docid, Version &ver): Version(ver), docid(_docid) {}

		virtual string toString();

	  virtual vector<unsigned char> serialize() {
		  vector<unsigned char> output = intToBytes(docid);
			vector<unsigned char> prev = Version::serialize();
	    output.insert(output.end(), prev.begin(), prev.end());
		  return output;
	  }

  protected:
    int docid;
};

class Document {
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
    Document(const Document &doc);
    ~Document();

    virtual void insertVersion(const Version &ver){}
    //virtual int searchVer(const Version &ver);
    //virtual int searchVer(const int query_time);
    virtual void sortList();


    virtual int getID() const { return docid; }
    virtual vector< Version > getVerList() const { return ver_list; }
    /*
    find the version that after the time point x
    @param:
      x: the time point
    @return:
      the first version position after x
    */
    virtual int binary_search(long long x);

    /*
    transform data into bytes stream
    @return:
      bytes stream
    */
    virtual vector<unsigned char> serialize();
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
    virtual vector< QueryVersion > query(long long st_time, long long ed_time, int top_k);

    /*
    load the document information from disk
    @param:
      fp: the pointer of the document in the file
    @return:
      the bytes of the document.
      if fail, return -1
    */
    virtual int load(FILE *fp);

    // Inherit from basic interval class
    // These function need to be implemented for interval tree use
    virtual int GetLowPoint() const ;
    virtual int GetHighPoint() const ;
    virtual long long getStartTime();
    virtual void Print();

  private:
    int docid;
    long long start_time;
    long long end_time;
    vector< Version > ver_list;
};

struct DocObject {
  int docid;
  int revid;
  int id;
  long long start_time;

  DocObject(){}
  DocObject(int _id): id(_id){}
  DocObject(int _id, int _docid, int _revid, long long st_time): 
    id(_id), docid(_docid), revid(_revid), start_time(st_time) {}

  
  bool operator < (const DocObject &t) const {
    return docid < t.docid || (docid == t.docid && revid < t.revid);
  }
  
  /*
  bool operator < (const DocObject &t) const {
    return id < t.id;
  }
  */
};

void buildDocSet(char *filename, set< DocObject > &docObj, int &doc_count);
DocObject getDocInfo(int id, set< DocObject > &docObj);
bool ver_cmp(const Version &, const Version &);
#endif

