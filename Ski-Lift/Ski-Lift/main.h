#pragma once
#include <set>
using namespace std;
void troubleshootingtests(int test_no);
template<class T>
bool in_set(set<T> container, T element);

#define LIFT_CAPACITY 101
#define MIN_WEIGHT 40
#define MAX_WEIGHT 100
#define TOKEN_MSG 0
#define RELEASE_MSG 1
#define REQUEST_MSG 2
#define PRIORITY_MSG 3
