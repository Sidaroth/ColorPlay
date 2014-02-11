
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
	WindowHandler();
	WindowHandler(std::string windowName, LogModule* logger, int width=800, int height=600, bool verticalSync=true, int frameRateLimit=60 );
	void run();

private:
	sf::RenderWindow window;
	LogModule* logger;
};