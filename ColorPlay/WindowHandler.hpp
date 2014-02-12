
/*
* Purpose: Handle the swapping, loading and unloading of windows / scenes. 
*
* Last edited: 10. Feb. 2014
*
* Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
*		   Gjøvik University College
* 
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "LogModule.hpp"


class WindowHandler
{
public:
	enum class WindowType{
		Config,
		Game,
		Shield
	};

	WindowHandler();
	WindowHandler(std::string windowName, LogModule* logger, bool* running, int width=800, int height=600, bool verticalSync=true, int frameRateLimit=60 );
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

	void gameProcessEvents();
	void gameUpdate();
	void gameRender();
	
	void configProcessEvents();
	void configUpdate();
	void configRender();
};