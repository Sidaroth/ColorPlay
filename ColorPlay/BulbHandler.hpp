/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 

	Last edited: 13. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gjøvik University College.
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

#include <curl/curl.h>
#include "StringQueue.hpp"
#include "EventQueue.hpp"
#include "ActionEvent.hpp"

#define DEBUG 1

class BulbHandler
{
public:
	BulbHandler();
	BulbHandler(EventQueue *eventQueue);

	typedef struct Bulb{
		short id;
		short hue;
		short sat;
		short bri;
		bool on; 
	} Bulb;


	void setBulbAdress(std::string bulbAdress);
	void setBrightness(int brightness, int bulbId);
	int  getBrightness(int bulbId);
	void setHue(int hue, int bulbId);
	int  getHue(int bulbId);
	void setSaturation(int saturation, int bulbId);
	int  getSaturation(int bulbId);

	void processEvents();

	static int callback_func(void *getInfo, size_t size, size_t count, void *stream);
	
private:

	CURL* curl;
	std::string bulbAdress;
	EventQueue *eventQueue;
	ActionEvent currentAction;

	void command(std::string body, int bulbId);
	void commandGet();

	int Hue;
};
