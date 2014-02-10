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

#define DEBUG 1

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "ColorPlay");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	BulbHandler bulbHandler;
	LogModule logger;

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

	std::thread loggerThread(&LogModule::run, &logger);	// Run the logger module in a background thread.
	//std::thread inputControlThread(&inputHandler::run, &inputHandler); // Something like this...

	//bulbHandler.runCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);
	//bulbHandler.getHue(1);
	// inputControlThread.join();

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::Black);
		window.draw(shape);
		window.display();
	}



	
	loggerThread.join(); // Wait for the background thread(s) to finish. 

	MoveHandler moveHandler;
	moveHandler.connect();

	std::cout << "\nFinished, press any key to exit.";
	getchar();
	return 0;	
}