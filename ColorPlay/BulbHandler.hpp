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
#include <iostream>
#include <curl/curl.h>

#define DEBUG 1
#define CURL_STATICLIB

class BulbHandler
{
public:
	BulbHandler();
	void setBulbAdress(char* bulbAdress);
	void addBulb(char id);

	void setBrightness(int brightness);
	int  getBrightness();
	void setHue(int hue);
	int  getHue();
	void setSaturation(int saturation);
	int  getSaturation();


private:
	std::vector<char> bulbList;
	std::vector<char>::iterator it;
	std::vector<char> messageString;

	CURL* curl;
	char* bulbAdress;

	char* buildBody(std::string message);
	void command(std::string body, char* type);
};
