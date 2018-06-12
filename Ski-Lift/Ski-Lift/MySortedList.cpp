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
		//exit(-1);
	}
	return *max_element(MyList.begin(), MyList.end(), MyLess);
}

Request MySortedList::pop()
{
	Request request = best();
	erase(request);
	return request;
}

Request MySortedList::simple_pop()
{
	std::list<Request>::iterator it = MyList.begin();
	Request request = *it;
	MyList.erase(it);
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

int MySortedList::decrease(Request element)
{
	std::list<Request>::iterator it;
	std::list<Request>::iterator forErase= MyList.end();
	for (it = MyList.begin(); it != MyList.end(); it++) {
		if (forErase != MyList.end()) {
			MyList.erase(forErase);
			forErase = MyList.end();
		}
		if (element == (*it)) {
			if (element.weight > it->weight) {
				forErase = it;
				element.weight -= it->weight;
			}
			else {
				it->weight -= element.weight;
				return 0;
			}
		}
	}
	if (forErase != MyList.end()) {
		MyList.erase(forErase);
		forErase = MyList.end();
	}
	return element.weight;
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

void MySortedList::increasePriorityExcept(int sender)
{
	std::list<Request>::iterator it;
	for (it = MyList.begin(); it != MyList.end(); it++) {
		if (it->id != sender) {
			it->priority++;
		}
		else {
			MyList.erase(it);
		}
	}
}
