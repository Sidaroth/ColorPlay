#include "LogModule.hpp"

LogModule::LogModule()
{
	running = true;
}

void LogModule::run()
{
	std::cout << "Logger starting up.\n";
	ofs.open("test.csv", std::ofstream::out | std::ofstream::app);
	
	std::chrono::time_point<std::chrono::system_clock> now;
	std::string eventDesc = "";

	// Polling the queue, sleeping to avoid CPU Drain (busy wait). 
	while (running)
	{
		if (!logEvents.empty()) // Is there a log event to pop off. 
		{
			eventDesc = logEvents.pop();

			now = std::chrono::system_clock::now();
			std::time_t logTime = std::chrono::system_clock::to_time_t(now);

			char buffer[BUFFERSIZE];
			ctime_s(buffer, BUFFERSIZE, &logTime);

			if (eventDesc == "quit")
			{
				running = false;
			}

			std::cout << "Log event: " << eventDesc << "\n";
			ofs << "Log event(" << buffer << "): " << eventDesc << "\n";
		}

		// Avoid busy wait while polling. 
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	std::cout << "Logger shutting down!";
}

void LogModule::LogEvent(std::string message)
{
	logEvents.push(message);
}