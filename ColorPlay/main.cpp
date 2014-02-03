#include <chrono>
#include <thread>
#include <conio.h>
#include <string>
#include <iostream>

#include <curl/curl.h>

#define DEBUG 1

void debugMessage(std::string message, int debugLevel);
void colorCalibration(int low = 0, int high = 65535, int step = 10, int stepDelay = 0);

int main(int argc, char* argv[])
{
	colorCalibration();
	_getch();
	return 0;	
}

void colorCalibration(int low, int high, int step, int stepDelay)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	if(DEBUG > 0)
	{
		start = std::chrono::system_clock::now();
	}

	for (int i = low; i <= high; i += step)
	{
		// Do the work.
		std::cout << "Hue: " << i << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay)); // Sleep for the stepDelay. 
	}

	if (DEBUG > 0)
	{
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "Calibration time: " << elapsed_seconds.count() << "s\n";
	}
}