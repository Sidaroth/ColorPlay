#include "StringQueue.hpp"

StringQueue::StringQueue()
{

}

void StringQueue::push(std::string message)
{
	std::lock_guard<std::mutex> lock(mutex); // RAII style lock

	data.push(message);
}

int StringQueue::size()
{
	std::lock_guard<std::mutex> lock(mutex);
	return data.size();
}

bool StringQueue::empty()
{
	std::lock_guard<std::mutex> lock(mutex);
	return data.empty();
}

std::string StringQueue::pop()
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