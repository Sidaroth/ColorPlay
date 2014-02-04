#include <chrono>
#include <thread>
#include <conio.h>
#include <string>
#include <iostream>
#include <sstream>

#include <curl/curl.h>
#include "BulbHandler.hpp"

#define DEBUG 1

void colorCalibration(int bulb = 1, int low = 0, int high = 65535, int step = 10, int stepDelay = 0);
int *testSettings();
BulbHandler bulbHandler;

int main(int argc, char* argv[])
{
	int *settings = testSettings();
	bulbHandler.setBulbAdress("http://192.168.37.114/api/newdeveloper/lights/");

	bulbHandler.addBulb('1');
	bulbHandler.addBulb('2');
	bulbHandler.addBulb('3');
	bulbHandler.addBulb('4');

	bulbHandler.runCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	//colorCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	
	std::cout << "\nFinished, press any key to exit.";
	_getch();
	return 0;	
}

int *testSettings()
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
		std::cout << "\nPlease enter the step delay in ms (0-1800000): ";
		std::cin >> settings[4];
	}

	return settings;
}