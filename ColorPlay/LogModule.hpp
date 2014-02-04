/*
	Purpose: This module receives events, and handles the logging of the program. 
			 This module will run in a 2nd thread, polling a queue for log events. 

	last edited: 4. Feb. 2014

	authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gjøvik University College.

*/

#pragma once
#include <mutex>
#include <iostream>

class LogModule
{
public:
	LogModule();
	void LogEvent(std::string message);
	void run();

private:
	std::mutex mutex;
};