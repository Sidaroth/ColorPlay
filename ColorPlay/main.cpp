#include <chrono>
#include <thread>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

#include "BulbHandler.hpp"
#include "LogModule.hpp"
#include "StringQueue.hpp"
#include "MoveHandler.hpp"
#include "WindowHandler.hpp"

#define DEBUG 1

int main(int argc, char* argv[])
{
	BulbHandler bulbHandler;
	LogModule logger;
	WindowHandler windowHandler;

	bulbHandler.setBulbAdress("http://192.168.1.172/api/newdeveloper/lights/");

	bulbHandler.addBulb('1');
	bulbHandler.addBulb('2');
	bulbHandler.addBulb('3');
	bulbHandler.addBulb('4');

	logger.LogEvent("Adding lightBulb 1");
	logger.LogEvent("Adding lightBulb 2");
	logger.LogEvent("Adding lightBulb 3");
	logger.LogEvent("Adding lightBulb 4");
	logger.LogEvent("quit");

	windowHandler.run();

	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	//std::thread inputControlThread(&inputHandler::run, &inputHandler); // Something like this...

	//bulbHandler.runCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	//bulbHandler.getHue(1);
	// inputControlThread.join();
	
	loggerThread.join(); // Wait for the background thread(s) to finish. 

	MoveHandler moveHandler;
	moveHandler.connect();

	std::cout << "\nFinished, press any key to exit.";
	getchar();
	return 0;	
}