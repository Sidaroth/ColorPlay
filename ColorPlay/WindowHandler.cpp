#include "WindowHandler.hpp"

WindowHandler::WindowHandler()
{
	window.create(sf::VideoMode(800, 600), "Default Name huehue");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
}

WindowHandler::WindowHandler(std::string windowName,
							 LogModule* logger,
							 bool* running,
							 int width,          /* 800  */ 
							 int height, 		 /* 600  */
							 bool verticalSync , /* true */ 
							 int frameRateLimit) /* 60   */
{
	window.create(sf::VideoMode(width, height), windowName);
	window.setVerticalSyncEnabled(verticalSync);
	
	this -> logger = logger;
	this -> running = running; 

	if(verticalSync != true)			// VSync and forced framerate should not be used at the same time. It can make the screen do weird things... - SFML Docs.
	{
		window.setFramerateLimit(frameRateLimit);
	}

	logger -> LogEvent("Window created");
	
}

bool WindowHandler::processEvents()
{
	while(window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
		{
			window.close();
			return true;
		}
	}

	return false;
}

void WindowHandler::update()
{

}

void WindowHandler::render()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Red);
	window.clear(sf::Color::Black);

	window.draw(shape);
	window.display();
}

void WindowHandler::close()
{
	window.close();
}