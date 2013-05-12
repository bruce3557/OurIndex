/*
 * Author: Bruce Kuo
 * Date: 2013.04.26
 * 
 * Description:
 *   Some basic library which does not include in built_in 
 *   funcitons
 */

#ifndef __BASE_UTIL_H__
#define __BASE_UTIL_H__

#include <vector>

using std::vector;

vector<unsigned char> intToBytes(int num) {
  // translate integer to byte array
  vector<unsigned char> array(4);
  for(int i=0;i<4;++i)
    array[3-i] = (num >> (i * 8));
  return array;
}

int bytesToInt(vector<unsigned char> &vec) {
  // translate byte array to integer
  int result = 0;
  for(int i=0;i<4;++i) {
    int x = vec[3-i];
    result |= x << (i * 8);
  }
  return result;
}

vector<unsigned char> longlongToBytes(long long num) {
  vector<unsigned char> array(8);
  for(int i=0;i<8;++i)
    array[7-i] = (num >> (i * 8));
  return array;
}

long long bytesToLonglong(vector<unsigned char> &vec) {
  long long result = 0;
  for(int i=0;i<8;++i) {
    long long x = vec[7-i];
    result |= x << (i * 8);
  }
  return result;
}

#endif