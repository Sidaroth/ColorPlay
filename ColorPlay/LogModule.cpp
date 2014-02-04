#include "LogModule.hpp"

LogModule::LogModule()
{

}

void LogModule::run(StringQueue* queue)
{
	std::cout << "Logger running" << "\n";

	while (!queue -> empty())
	{
		std::cout << "Log event: " << queue -> pop() << "\n";
	}
}