/*
* Purpose: Handle the swapping, loading and unloading of windows / scenes. 
*
* Last edited: 21. Feb. 2014
*
* Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
*		   Gjøvik University College
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "LogModule.hpp"
#include "BulbHandler.hpp"
#include "Timer.hpp"

class WindowHandler
{
public:
	enum class WindowType{
		Config,
		Game,
		Shield		// For shield pattern
	};

	WindowHandler();
	WindowHandler(	std::string windowName,
					LogModule* logger,
					bool* running,
					BulbHandler* bulbHandler,
					bool* finished,
					bool* newGame,
					int width=1280,
					int height=720,
					bool verticalSync=true,
					int frameRateLimit=60
				);
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
	int textSize;
	unsigned int edgeOffset = 5;

	Timer timer;
	bool* finished;
	bool* newGame;

	int currentInstruction;
	std::vector<std::wstring> instructions;

	BulbHandler* bulbHandler;

	int width;
	int height;

	void gameProcessEvents();
	void gameUpdate();
	
	void gameRender();
	void renderGoalColor();
	void renderInstructions();
	void renderScore();
	sf::String wrapText(sf::String string, unsigned width, const sf::Font &font, unsigned charicterSize, bool bold = false);

	void configProcessEvents();
	void configUpdate();
	void configRender();
};