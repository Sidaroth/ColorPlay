/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 

	Last edited: 4. Feb. 2014

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
#include <curl/curl.h>
#include "StringQueue.hpp"

#define DEBUG 1
#define CURL_STATICLIB

class BulbHandler
{
public:
	BulbHandler();
	void setBulbAdress(char* bulbAdress);
	void addBulb(char id);

	void setBrightness(int brightness, int bulbId);
	int  getBrightness(int bulbId);
	void setHue(int hue, int bulbId);
	int  getHue(int bulbId);
	void setSaturation(int saturation, int bulbId);
	int  getSaturation(int bulbId);

	void runCalibration(int bulbId, int low, int high, int step, int stepDelay);
	
private:
	std::vector<char> bulbList;
	std::vector<char>::iterator it;

	CURL* curl;
	char* bulbAdress;

	char* buildBody(std::string message);
	void command(std::string body, char* type, int bulbId);
};
