#pragma once
#include <list>
#include "Request.h"
//#include <algorithm>

class MySortedList
{
public:
	MySortedList();
	~MySortedList();
	std::list<Request>MyList;
	Request best();
	Request pop();
	Request simple_pop();
	void insert(Request element);
	bool erase(Request element);
	bool contains(Request element);
	bool empty();
	int size();
	void increasePriority();
	void increasePriorityExcept(int sender);
	int decrease(Request element)
};

