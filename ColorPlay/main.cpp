#include <chrono>
#include <thread>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>


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
	WindowHandler windowHandler("Color Play Game v.0.1", &logger);

	bulbHandler.setBulbAdress("http://192.168.1.172/api/newdeveloper/lights/");

	bulbHandler.addBulb('1');
	bulbHandler.addBulb('2');
	bulbHandler.addBulb('3');
	bulbHandler.addBulb('4');

	logger.LogEvent("Adding lightBulb 1");
	logger.LogEvent("Adding lightBulb 2");
	logger.LogEvent("Adding lightBulb 3");
	logger.LogEvent("Adding lightBulb 4");
	

	///////////////// START THREADS /////////////////
	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	//std::thread inputControlThread(&inputHandler::run, &inputHandler); // Something like this...


	///////////////// START WORK IN THE MAIN THREAD //////////////////
	windowHandler.run();

	//bulbHandler.runCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	//bulbHandler.getHue(1);


	// UNCOMMENT BEFORE GIT COMMIT. IN MOVEHANDLER AND MAKEFILE
	//MoveHandler moveHandler;
	//moveHandler.connect();

	logger.LogEvent("quit"); // logger.quit(); perhaps...
	///////////////// END WORK IN MAIN THREAD //////////////////////


	///////////////// JOIN / WAIT FOR THREADS - NO MORE WORK AFTER THIS/////////////////////
	// inputControlThread.join();
	loggerThread.join(); // Wait for the background thread(s) to finish. 

	std::cout << "\nFinished, press any key to exit.";
	getchar();
	return 0;	
}