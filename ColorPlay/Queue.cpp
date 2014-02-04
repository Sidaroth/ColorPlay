#include "Queue.hpp"

Queue::Queue()
{

}

void Queue::push(std::string message)
{
	std::lock_guard<std::mutex> lock(mutex); // RAII style lock

	data.push(message);
}

int Queue::size()
{
	std::lock_guard<std::mutex> lock(mutex);
	return data.size();
}

bool Queue::empty()
{
	std::lock_guard<std::mutex> lock(mutex);
	return data.empty();
}

std::string Queue::pop()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (!data.empty())
	{
		std::string message = data.front();
		data.pop();
		return message;
	}

	else return "";
}