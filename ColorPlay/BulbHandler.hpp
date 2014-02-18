/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 

	Last edited: 17. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gj�vik University College.
*/

#pragma once // Include guard

#include <vector>
#include <string.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <istream>
#include <ostream>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector3.hpp>
#include <curl/curl.h>
#include "EventQueue.hpp"
#include "ActionEvent.hpp"
#include "LogModule.hpp"
#include "globals.hpp"

#define SEED 314159265358979323
#define DEBUG 1

class BulbHandler
{
public:
	BulbHandler();
	BulbHandler(EventQueue *eventQueue, LogModule* logger);

	// All bulbs use HSV color space, SFML works with RGB.
	// The system will use one of the following. 
	enum class ColorSpace
	{
		Lab,
		XYZ,
		CMY,
		RGB,
		HSV
	};

	// Will this struct be used? - Remains to be seen.
	typedef struct Bulb{
		short id;
		short hue;
		short sat;
		short bri;
		bool on; 
	} Bulb;


	void generateNewGoalColor();

	void setBulbAdress(std::string bulbAdress);
	void setBrightness(int brightness, int bulbId);
	void setHue(int hue, int bulbId);
	void setSaturation(int saturation, int bulbId);

	void setColorSpace(ColorSpace colorSpace);
	void setGoalColor(sf::Color color);
	void setGoalColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a=255);

	sf::Color getGoalColor();

	void setVariables(int bulbId);

	void processEvents();

private:

	sf::Color goalColor;
	CURL* curl;
	std::string bulbAdress;
	EventQueue *eventQueue;
	ActionEvent currentAction;
	ColorSpace currentColorSpace;

	LogModule* logger;

	std::mt19937 gen;
	std::uniform_int_distribution<> rgbDistribution;

	void command(std::string body, int bulbId);
};
