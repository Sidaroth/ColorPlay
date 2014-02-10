
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


class WindowHandler
{
public:
	WindowHandler();
	void run();

private:
	sf::RenderWindow window;
};