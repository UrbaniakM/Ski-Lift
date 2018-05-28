#pragma once
class TimeoutManager
{
public:
	TimeoutManager();
	~TimeoutManager();
	void reset();
	bool check();
};

