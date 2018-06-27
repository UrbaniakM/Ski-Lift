#pragma once

struct Request {
	int priority;
	int weight;
	int id;
	bool correct = false;
};

struct TokensStruct {
	int tokens;
	int target;
	bool correct = false;
};
