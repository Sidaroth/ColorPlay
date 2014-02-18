#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <sstream>

#include "globals.hpp"
#include "BulbHandler.hpp"
#include "BulbMath.hpp"
#include "LogModule.hpp"
#include "StringQueue.hpp"
#include "MoveHandler.hpp"
#include "WindowHandler.hpp"
#include "EventQueue.hpp"

#define DEBUG 1

//tHis isn't even here
sf::Vector3f Bulb1HSV(0,0,0);
sf::Vector3f Bulb2HSV(0,0,0);
sf::Vector3f Bulb3HSV(0,0,0);


int main(int argc, char* argv[])
{
	bool running = false;
	LogModule logger;
	EventQueue eventQueue;
	BulbMath bulbMath;

	BulbHandler bulbHandler(&eventQueue, &logger);
	MoveHandler moveHandler(&logger, &bulbHandler, &eventQueue, &running);
	WindowHandler windowHandler("Color Play Game v.0.1", &logger, &running, &bulbHandler);

	////////////////////// INIT //////////////////////
	std::string url = "http://192.168.37.114/api/newdeveloper/lights/";

	bulbHandler.setBulbAdress(url);

	bulbMath.lab2xyz(20.0f, 120.0f, 50.0f);

	//bulbHandler.setVariables(2);
	
	std::cout << "\n----------->" << Bulb1HSV.x << "<----------" << std::endl;
	if(windowHandler.init())
	{
		std::cout << "Window initialization failed! Exiting...\n";
		return -1;
	}
	else
	{
		std::cout << "Window initialization successful!\n";
	}

	bulbHandler.setHue(20000, 1);
	bulbHandler.setBrightness(126, 1);
	bulbHandler.setHue(40000, 2);
	bulbHandler.setBrightness(126, 2);
	bulbHandler.setHue(60000, 3);
	bulbHandler.setBrightness(126, 3);
	//bulbHandler.setHue(56000, 4);

	///////////////// START THREADS /////////////////
	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	std::thread moveHandlerThread(&MoveHandler::run, &moveHandler);
	
	///////////////// START WORK IN THE MAIN THREAD //////////////////
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	running = true;
	bulbHandler.setVariables(1);
	bulbHandler.setVariables(2);
	bulbHandler.setVariables(3);
	bulbHandler.setColorSpace(BulbHandler::ColorSpace::RGB);

	ActionEvent event(12, 1, ActionEvent::Action::Up);
	eventQueue.push(event);
	eventQueue.push(event);
	eventQueue.push(event);

	event.setAction(ActionEvent::Action::Down);
	event.setBulbID(2);
	eventQueue.push(event);
	eventQueue.push(event);
	eventQueue.push(event);

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