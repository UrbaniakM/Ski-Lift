#include "MySortedList.h"
#include <algorithm>
#include <list>
#include <stdexcept>
#include <stdlib.h>
#include "Request.h"
using namespace std;

bool MyLess(Request r1, Request r2) {
	return r1.priority < r2.priority;
}

MySortedList::MySortedList()
{
}


MySortedList::~MySortedList()
{
}

Request MySortedList::best()
{
	if (MyList.empty()) {
		throw std::invalid_argument("MySortedList.best(): MySortedList.MyList is empty");
		exit(-1);
	}
	return *max_element(MyList.begin(), MyList.end(), MyLess);
}

Request MySortedList::pop()
{
	Request request = best();
	erase(request);
	return request;
}

void MySortedList::insert(Request element)
{
	MyList.push_back(element);
}

bool MySortedList::erase(Request element)
{
	std::list<Request>::iterator it;
	it = std::find(MyList.begin(), MyList.end(), element);
	if (it == MyList.end()) {
		return false;
	}
	else {
		MyList.erase(it);
		return true;
	}
}

bool MySortedList::contains(Request element)
{
	std::list<Request>::iterator it;
	it = std::find(MyList.begin(), MyList.end(), element);
	if (it == MyList.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool MySortedList::empty()
{
	return MyList.empty();
}

int MySortedList::size()
{
	return MyList.size();
}

void MySortedList::increasePriority()
{
	std::list<Request>::iterator it;
	for (it = MyList.begin(); it != MyList.end(); it++) {
		it->priority++;
	}
}
