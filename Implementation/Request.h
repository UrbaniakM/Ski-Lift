#pragma once

struct Request {
	int priority;
	int weight;
	int id;
	bool correct = false;
};

struct PriorityIncrement {
	int newPriority;
	int id;
	bool correct = false;
};
