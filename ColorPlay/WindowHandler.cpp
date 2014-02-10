#include "WindowHandler.hpp"

WindowHandler::WindowHandler()
{
	window.create(sf::VideoMode(800, 600), "Color Play Game v.0.1");
}

void WindowHandler::run()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Green);

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