/*
	Purpose: This module receives events, and handles the logging of the program. 
			 This module will run in a 2nd thread, polling a queue for log events. 

	last edited: 7. Feb. 2014

	authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gjøvik University College.

*/

#pragma once
#include <mutex>
#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <chrono>
#include "StringQueue.hpp"

#define BUFFERSIZE 26

class LogModule
{
public:
	LogModule();
	void LogEvent(std::string message);
	void run();

private:
	std::mutex mutex;
	std::ofstream ofs;
	StringQueue logEvents;

	bool running;
};