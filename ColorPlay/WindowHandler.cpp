#include "WindowHandler.hpp"

WindowHandler::WindowHandler()
{
	window.create(sf::VideoMode(800, 600), "Default Name huehue");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
}

WindowHandler::WindowHandler(std::string windowName,
							 LogModule* logger, 
							 int width,          /* 800  */ 
							 int height, 		 /* 600  */
							 bool verticalSync , /* true */ 
							 int frameRateLimit) /* 60   */
{
	window.create(sf::VideoMode(width, height), windowName);
	window.setVerticalSyncEnabled(verticalSync);
	
	this -> logger = logger;

	if(verticalSync != true)			// VSync and forced framerate should not be used at the same time. It can make the screen do weird things... - SFML Docs.
	{
		window.setFramerateLimit(frameRateLimit);
	}

	logger -> LogEvent("Window created");
	
}

// The window handler / render loop of the game.  
void WindowHandler::run()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Green);

	logger -> LogEvent("WindowHandler running");
	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			// If either the "cross" is clicked, or the escape button is pressed. 
			if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			{
				window.close();
			}
		}

		// Clear the window with BG color.
		window.clear(sf::Color::Black);
		
		// Draw everything onto the screen buffer.
		window.draw(shape);

		// Swap the buffer  / actually draw the screen. 
		window.display();
	}
}