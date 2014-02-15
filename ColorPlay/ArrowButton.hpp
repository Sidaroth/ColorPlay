/*
*	Purpose: Serve as an arrow to select options in the game (options etc.)
*
*	Last edit: 13. Feb. 2014
*
*	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun. 
*			 Gjøvik University College. 
*
*/
#pragma once

#include <SFML/Graphics.hpp>

class ArrowButton
{
public:
	ArrowButton();
	ArrowButton(sf::Vector2f pos);
	ArrowButton(std::string id, sf::Vector2f pos);

	enum class Direction{
		Left,
		Right
	};

	void setPos(sf::Vector2f pos);
	void setPos(float x, float y);
	void setPos(sf::Vector2f &other);

	sf::Vector2f getPos();

	void setId(std::string id);
	std::string getId();

	void setDirection(Direction direction);
	void swapDirection();

private:
	std::string id; 
	sf::Vector2f pos;

	Direction direction;
};