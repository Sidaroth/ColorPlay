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
#include "EventQueue.hpp"

#define DEBUG 1

int main(int argc, char* argv[])
{
	bool running = false;

	EventQueue eventQueue;
	BulbHandler bulbHandler(&eventQueue);
	BulbMath bulbMath;
	LogModule logger;
	MoveHandler moveHandler(&logger, &running);
	WindowHandler windowHandler("Color Play Game v.0.1", &logger, &running);

	////////////////////// INIT //////////////////////
	std::string url = "http://192.168.37.114/api/newdeveloper/lights/";

	bulbHandler.setBulbAdress(url);
	
	bulbMath.rgb2xyz(200.0f, 255.0f, 0.0f);

	if(windowHandler.init())
	{
		std::cout << "Window initialization failed! Exiting...";
		return -1;		
	}

	//bulbHandler.setHue(46000, 1);
	bulbHandler.setHue(60000, 2);
	bulbHandler.setHue(61000, 3);
	//bulbHandler.setHue(56000, 4);

	///////////////// START THREADS /////////////////
	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	std::thread moveHandlerThread(&MoveHandler::run, &moveHandler);
	
	///////////////// START WORK IN THE MAIN THREAD //////////////////
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	running = true;

	ActionEvent event(12, ActionEvent::Action::Up);
	eventQueue.push(event);
	event.setAction(ActionEvent::Action::None);
	eventQueue.push(event);
	event.setAction(ActionEvent::Action::Finish);
	eventQueue.push(event);
	event.setAction(ActionEvent::Action::Down);
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
	
	std::cout << "\nFinished, press any key to exit.";
	getchar();
	return 0;	
}