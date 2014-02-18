#pragma once

#include <chrono>
 
class Timer
{
public:
	Timer();

	void start();
	int secondsElapsed() const;

private:
	std::chrono::high_resolution_clock::time_point epoch;
};