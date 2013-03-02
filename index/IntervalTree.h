#include <stdio.h>
#include <string.h>
#include <vector.h>

#include "Document.h"

using std::vector;

#ifndef __INTERVALTREE_H__
#define __INTERVALTREE_H__

class IntervalTreeNode {
public:
  IntervalTreeNode();
  IntervalTreeNode(bool _red);
  void setLeft(IntervalTreeNode *ptr);
  void setRight(IntervalTreeNode *ptr);
  IntervalTreeNode *getLeft();
  IntervalTreeNode *getRight();

private:
  bool red;
  IntervalTreeNode *left;
  IntervalTreeNode *right;
  IntervalTreeNode *parent;
  vector< Document > doc_list;
};

class IntervalTree {

public:
  IntervalTree();

  void insertDoc(Document *doc);


private:
  IntervalTree *root;
  IntervalTree *nil;

  void leftRotate();
  void rightRotate();
  void recolor();
};

#endif
