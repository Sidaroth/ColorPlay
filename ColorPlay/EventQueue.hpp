/*
	Purpose: Serve as a thread-safe queue for ActionEvents, to enable communication between threads.
			 Wrapper class around std::queue

	Last edit: 14. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gjøvik University College

*/

#pragma once

#include <mutex>
#include <iostream>
#include <queue>
#include "ActionEvent.hpp"


class EventQueue
{
public:
	EventQueue();
	void push(ActionEvent event);
	bool empty();
	int size();
	ActionEvent pop();

private:
	std::mutex mutex;
	std::queue<ActionEvent> events;
};