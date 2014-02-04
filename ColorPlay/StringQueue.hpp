/*
	Purpose: Provide a thread safe queue for inter-thread communication of strings. 

	Last edited: 4. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gjøvik University College.
*/

#pragma once

#include <string>
#include <mutex>
#include <queue>

class StringQueue
{
public:
	StringQueue();
	void push(std::string message);
	bool empty();
	int  size();
	std::string pop();

private:
	std::mutex mutex;
	std::queue<std::string> data;
};