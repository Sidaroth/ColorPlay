/*
	Purpose: This class is used to determine how much time has passed since an event took place.

	Last edited: 21. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gj�vik University College.
*/
#pragma once

#include <chrono>
 
class Timer
{
public:
	Timer();

	void start();
	void stop();
	bool isRunning();
	int secondsElapsed() const;

private:
	std::chrono::high_resolution_clock::time_point epoch;
	bool running;
};