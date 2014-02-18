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
	char buffer[BUFFERSIZE];
	struct tm * timeinfo;
	std::time_t logTime;

	// Polling the queue, sleeping to avoid CPU Drain (busy wait). 
	while (running)
	{
		//std::cout << "Logger thread: " << std::this_thread::get_id() << std::endl;
		if (!logEvents.empty()) // Is there a log event to pop off. 
		{
			eventDesc = logEvents.pop();

			now = std::chrono::system_clock::now();
			logTime = std::chrono::system_clock::to_time_t(now);

			time(&logTime);
			timeinfo = localtime(&logTime);
			strftime(buffer, BUFFERSIZE, "%F,%T",timeinfo);


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

	std::cout << "Logger shutting down!\n";
}

void LogModule::LogEvent(std::string message)
{
	logEvents.push(message);
}