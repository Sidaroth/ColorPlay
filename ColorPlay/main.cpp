#include <chrono>
#include <thread>
#include <conio.h>
#include <string>
#include <iostream>
#include <sstream>

#include <curl/curl.h>
#include "BulbHandler.hpp"
#include "LogModule.hpp"
#include "StringQueue.hpp"

#define DEBUG 1

void colorCalibration(int bulb = 1, int low = 0, int high = 65535, int step = 10, int stepDelay = 0);
int *getTestSettings();//Asks the user for the test parameters

int main(int argc, char* argv[])
{
	BulbHandler bulbHandler;
	LogModule logger;
	StringQueue logEvents;


	//int *settings = getTestSettings();
	bulbHandler.setBulbAdress("http://192.168.37.114/api/newdeveloper/lights/");

	bulbHandler.addBulb('1');
	bulbHandler.addBulb('2');
	bulbHandler.addBulb('3');
	bulbHandler.addBulb('4');

	//std::thread loggerThread(&LogModule::run, &logger, &logEvents);	// Run the logger module in a background thread.
	//std::thread inputControlThread(&inputHandler::run, &inputHandler); // Something like this...

	//bulbHandler.runCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	bulbHandler.getHue(1);
	//loggerThread.join(); // Wait for the background thread(s) to finish. 
	// inputControlThread.join();
	std::cout << "\nFinished, press any key to exit.";
	_getch();
	return 0;	
}

//Asks the user for the test parameters
int *getTestSettings()
{
	int *settings = new int[5];

	while (settings[0] < 1 || 4 < settings[0])
	{
		std::cout << "\nPlease enter the number of the bulb (1-4): ";
		std::cin >> settings[0];
	}
	while (settings[1] < 0 || 65535 < settings[1])
	{
		std::cout << "\nPlease enter the low hue threshold (0-65535): ";
		std::cin >> settings[1];
	}
	while (settings[2] < 0 || 65535 < settings[2])
	{
		std::cout << "\nPlease enter the high hue threshold (0-65535): ";
		std::cin >> settings[2];
	}
	while (settings[3] < 1 || 10000 < settings[3])
	{
		std::cout << "\nPlease enter the step increment (1-10000): ";
		std::cin >> settings[3];
	}
	while (settings[4] < 0 || 65535 < settings[4])
	{
		std::cout << "\nPlease enter the step delay in ms (0-1800000): "; //0ms to 30min
		std::cin >> settings[4];
	}

	return settings;
}