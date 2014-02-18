#include "Timer.hpp"

Timer::Timer()
{

}

void Timer::start()
{ 
	epoch = std::chrono::high_resolution_clock::now(); 
}

int Timer::secondsElapsed() const
{
	const auto elapsed = (std::chrono::high_resolution_clock::now() - epoch);
	int seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

	return seconds;
}