#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <sstream>


#include "BulbHandler.hpp"
#include "BulbMath.hpp"
#include "LogModule.hpp"
#include "StringQueue.hpp"
#include "MoveHandler.hpp"
#include "WindowHandler.hpp"

#define DEBUG 1

int main(int argc, char* argv[])
{
	bool running = false;

	BulbHandler bulbHandler;
	BulbMath bulbMath;
	LogModule logger;
	MoveHandler moveHandler(&logger, &running);
	WindowHandler windowHandler("Color Play Game v.0.1", &logger, &running);


	bulbHandler.setBulbAdress("http://192.168.1.172/api/newdeveloper/lights/");

	bulbHandler.addBulb('1');
	bulbHandler.addBulb('2');
	bulbHandler.addBulb('3');
	bulbHandler.addBulb('4');

	logger.LogEvent("Adding lightBulb 1");
	logger.LogEvent("Adding lightBulb 2");
	logger.LogEvent("Adding lightBulb 3");
	logger.LogEvent("Adding lightBulb 4");
	
	bulbMath.lab2xyz(1,1,1);

	///////////////// START THREADS /////////////////
	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	std::thread moveHandlerThread(&MoveHandler::run, &moveHandler);
	
	///////////////// START WORK IN THE MAIN THREAD //////////////////
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	running = true; 

	while(running)
	{
		// Event processing

		windowHandler.processEvents();
		
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
	
	std::cout << "\nFinished, press any key to exit.";
	getchar();
	return 0;	
}