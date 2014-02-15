#include "EventQueue.hpp"

EventQueue::EventQueue()
{

}

void EventQueue::push(ActionEvent event)
{
	std::lock_guard<std::mutex> lock(mutex);

	events.push(event);
}

bool EventQueue::empty()
{
	std::lock_guard<std::mutex> lock(mutex);

	return events.empty();
}

int EventQueue::size()
{
	std::lock_guard<std::mutex> lock(mutex);

	return events.size();
}


// Combination of std::queue.front(), and std::queue.pop() for thread-safe FIFO functionality. 
ActionEvent EventQueue::pop()
{
	std::lock_guard<std::mutex> lock(mutex);

	if(events.empty())
	{
		std::cerr << "EventQueue.pop() Called on empty queue!" << std::endl;
		return NULL; 
	}

	ActionEvent event = events.front();
	events.pop();

	return event;
}