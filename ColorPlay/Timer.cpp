/*
	Purpose: This class is used to determine how much time has passed since an event took place.

	Last edited: 21. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gj�vik University College.
*/
#include "Timer.hpp"

Timer::Timer()
{
	this->running = false;
}

void Timer::start()
{ 
	this->epoch = std::chrono::high_resolution_clock::now(); 
	this->running = true;
}

void Timer::stop()
{
	this->running = false;
}

bool Timer::isRunning()
{
	return this->running;
}

int Timer::secondsElapsed() const
{
	const auto elapsed = (std::chrono::high_resolution_clock::now() - this->epoch);
	int seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

	return seconds;
}
