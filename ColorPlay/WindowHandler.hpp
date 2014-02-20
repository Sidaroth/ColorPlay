
/*
* Purpose: Handle the swapping, loading and unloading of windows / scenes. 
*
* Last edited: 18. Feb. 2014
*
* Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
*		   Gjøvik University College
* 
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "LogModule.hpp"
#include "BulbHandler.hpp"


class WindowHandler
{
public:
	enum class WindowType{
		Config,
		Game,
		Shield		// For shield pattern
	};

	WindowHandler();
	WindowHandler(std::string windowName, LogModule* logger, bool* running, BulbHandler* bulbHandler,int width=1280, int height=720, bool verticalSync=true, int frameRateLimit=60 );
	bool init(); 
	void processEvents();
	void update();
	void render();
	void close();

	void setWindowType(WindowType windowType=WindowType::Game);

private:
	sf::RenderWindow window;
	LogModule* logger;
	bool* running; 
	sf::Event event;
	WindowType windowType;
	sf::Font font;
	sf::Text text;

	BulbHandler* bulbHandler;

	int width;
	int height;

	void gameProcessEvents();
	void gameUpdate();
	
	void gameRender();
	void renderGoalColor();
	void renderInstructions();
	sf::String wrapText(sf::String string, unsigned width, const sf::Font &font, unsigned charicterSize, bool bold = false);

	void configProcessEvents();
	void configUpdate();
	void configRender();
};