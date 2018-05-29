/*
#include "main.h"
#include "Skier.h"
#include "MySortedList.h"
#include "Request.h"
#include <iostream>

void troubleshootingtests(int test_no)
{
	switch (test_no)
	{
	case 0:
		MySortedList msl;
		msl.insert(Request::make(1, 1, 0));
		msl.insert(Request::make(4, 2, 1));
		msl.insert(Request::make(2, 3, 2));
		while (!msl.empty()) {
			Request r = msl.pop();
			std::cout << r.priority << std::endl << r.weight << std::endl;
		}
		break;
	}
	return;
}
*/