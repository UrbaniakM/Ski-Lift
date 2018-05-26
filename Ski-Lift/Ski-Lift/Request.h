#pragma once

struct Request {
	int priority;
	int weight;
	int id;
	bool operator<(Request request);
	bool operator==(Request request);
	//Do sortowania ��da�
	Request();
	Request(int p, int w, int i);
	static Request make();
	static Request make(int p, int w, int i);
	bool correct=true;
};