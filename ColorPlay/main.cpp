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

int main(int argc, char* argv[])
{
	bool running = false;
	bool finished = false;
	bool newGame = false;
	LogModule logger;
	EventQueue eventQueue;
	BulbMath bulbMath;

	BulbHandler bulbHandler(&eventQueue, &logger, &finished, &newGame);
	//MoveHandler moveHandler(&logger, &bulbHandler, &eventQueue, &running);
	//WindowHandler windowHandler("Color Play Game", &logger, &running, &bulbHandler, &finished, &newGame);

	////////////////////// INIT //////////////////////
	std::string url = "http://192.168.1.57/api/newdeveloper/lights/";

	bulbHandler.setBulbAdress(url);
	bulbHandler.init();	 				// Load goal colors from file...

	// if(windowHandler.init())
	// {
	// 	std::cout << "Window initialization failed! Exiting...\n";
	// 	return -1;
	// }
	// else
	// {
	// 	std::cout << "Window initialization successful!\n";
	// }

	// ///////////////// START THREADS /////////////////
	// std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	// std::thread moveHandlerThread(&MoveHandler::run, &moveHandler);
	
	///////////////// START WORK IN THE MAIN THREAD //////////////////
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	running = true;

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	bulbHandler.setColorSpace(BulbHandler::ColorSpace::HSV);

	while(running)
	{
		// Event processing
		//windowHandler.processEvents();
		bulbHandler.processEvents();

		// Updates
		//windowHandler.update();
		
		// Rendering
		//windowHandler.render();
	}
	
		
	///////////////// CLEANUP //////////////////////
	//logger.LogEvent("quit"); // logger.quit(); perhaps...

	///////////////// JOIN / WAIT FOR THREADS - NO MORE WORK AFTER THIS/////////////////////
	//moveHandlerThread.join();
	//loggerThread.join(); // Wait for the background thread(s) to finish. 
	
	return 0;	
}
