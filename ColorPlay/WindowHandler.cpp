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

	windowType = WindowType::Game;

	logger -> LogEvent("Window created");
	
}

bool WindowHandler::init()
{
	if(!font.loadFromFile("./assets/Leo Arrow.ttf"))
	{
		this -> logger -> LogEvent("ERROR: Loading font failed!");
		return 1;
	}

	text.setFont(font);

	return 0;
}

void WindowHandler::processEvents()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameProcessEvents();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configProcessEvents();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler processEvents could not identify WindowType!");
	}
}

// Identifies the windowType and runs the appropriate update call. 
void WindowHandler::update()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameUpdate();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configUpdate();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler update could not identify WindowType!");
	}
}

/// Identifies the windowType and runs the appropriate render call. 
void WindowHandler::render()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameRender();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configRender();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler render could not identify WindowType!");
	}

}

void WindowHandler::close()
{
	window.close();
}

void WindowHandler::setWindowType(WindowType windowType)
{
	this -> windowType = windowType;
}



///////////// GAME RELATED /////////////////
void WindowHandler::gameProcessEvents()
{
	while(window.pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::Closed:
				window.close();
				(*running) = false;
				break;

			case sf::Event::KeyPressed:
			// Switch to determine which button is pressed. 
				switch(event.key.code)
				{
					case sf::Keyboard::Escape:
						window.close();
						(*running) = false;
						break;

					case sf::Keyboard::Space:
						this -> logger -> LogEvent("WindowType Changed to Config!");
						this -> windowType = WindowType::Config;
				
					default: break;
				}

			default: break;
		}
	}
}

void WindowHandler::gameUpdate()
{

}

void WindowHandler::gameRender()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Red);
	text.setString("Game Render!");
	text.setCharacterSize(24);
	text.setColor(sf::Color::Green);
	text.setPosition(500, 0);

	window.clear(sf::Color::Black);
	window.draw(shape);
	window.draw(text);
	window.display();
}


///////////////// CONFIG RELATED ////////////////
void WindowHandler::configProcessEvents()
{
	while(window.pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::Closed:
				window.close();
				(*running) = false;
				break;

			case sf::Event::KeyPressed:
			// Switch to determine which button is pressed. 
				switch(event.key.code)
				{
					case sf::Keyboard::Escape:
						window.close();
						(*running) = false;
						break;

					case sf::Keyboard::Space:
						this -> logger -> LogEvent("WindowType Changed to Game!");
						this -> windowType = WindowType::Game;
				
					default: break;
				}

			default: break;
		}
	}
}

void WindowHandler::configUpdate()
{

}

void WindowHandler::configRender()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Blue);
	text.setString("Config Render!");
	text.setCharacterSize(24);
	text.setColor(sf::Color::Black);
	text.setPosition(500, 0);

	window.clear(sf::Color::White);
	window.draw(shape);
	window.draw(text);
	window.display();
}