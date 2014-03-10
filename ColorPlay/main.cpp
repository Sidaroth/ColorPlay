#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "globals.hpp"
#include "BulbHandler.hpp"
#include "BulbMath.hpp"
#include "LogModule.hpp"
#include "StringQueue.hpp"
#include "MoveHandler.hpp"
#include "WindowHandler.hpp"
#include "EventQueue.hpp"
#include "Timer.hpp"

#define DEBUG 1

//Initializing globals, pretend like you don't see this
//sf::Vector3f Bulb1HSV(0,0,0);
//sf::Vector3f Bulb2HSV(0,0,0);
//sf::Vector3f Bulb3HSV(0,0,0);
//sf::Vector3f Bulb4HSV(0,0,0);
//bool isSetVariablesUpdated = false;
//std::vector<std::string> lol{""};



int main(int argc, char* argv[])
{
	bool running = false;
	bool finished = false;
	bool newGame = false;
	LogModule logger;
	EventQueue eventQueue;
	BulbMath bulbMath;

	BulbHandler bulbHandler(&eventQueue, &logger, &finished, &newGame);
	MoveHandler moveHandler(&logger, &bulbHandler, &eventQueue, &running);
	WindowHandler windowHandler("Color Play Game v.0.1", &logger, &running, &bulbHandler, &finished, &newGame);

	////////////////////// INIT //////////////////////
	std::string url = "http://192.168.1.172/api/newdeveloper/lights/";

	bulbHandler.setBulbAdress(url);

	bulbMath.lab2xyz(10.0f, -50.0f, 10.0f);

//	bulbHandler.setVariables(2);
	
	if(windowHandler.init())
	{
		std::cout << "Window initialization failed! Exiting...\n";
		return -1;
	}
	else
	{
		std::cout << "Window initialization successful!\n";
	}

	bulbHandler.setHue(0, 1);
	bulbHandler.setHue(0, 2);
	bulbHandler.setHue(0, 3);
	bulbHandler.setSaturation(255, 1);
	bulbHandler.setSaturation(255, 2);
	bulbHandler.setSaturation(255, 3);
	bulbHandler.setBrightness(100, 1);
	bulbHandler.setBrightness(100, 2);
	bulbHandler.setBrightness(100, 3);


	///////////////// START THREADS /////////////////
	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	std::thread moveHandlerThread(&MoveHandler::run, &moveHandler);
	
	///////////////// START WORK IN THE MAIN THREAD //////////////////
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	running = true;

	bulbHandler.setVariables(1);
	bulbHandler.setVariables(2);
	bulbHandler.setVariables(3);
	bulbHandler.setVariables(4);

/*
	//This timer has to be started when the user starts a new game.
	Timer timer;
	timer.start();

	bulbHandler.calculateScore(timer);
*/
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	bulbHandler.setColorSpace(BulbHandler::ColorSpace::HSV);

	while(running)
	{
		// Event processing
		windowHandler.processEvents();
		bulbHandler.processEvents();

		// Updates
		windowHandler.update();
		
		// Rendering
		windowHandler.render();
	}
	
		
	///////////////// CLEANUP //////////////////////
	logger.LogEvent("quit"); // logger.quit(); perhaps...

	///////////////// JOIN / WAIT FOR THREADS - NO MORE WORK AFTER THIS/////////////////////
	moveHandlerThread.join();
	loggerThread.join(); // Wait for the background thread(s) to finish. 
	
	return 0;	
}
