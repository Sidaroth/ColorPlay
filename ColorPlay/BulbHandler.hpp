/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 
			 as well as controlling the system concerned with distuinguishing the colorspaces and lightbulb cooperation. 
	
	Last edited: 13. Mar. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
		 Gj�vik University College.

	Warning: This has become quite the "Blob" class. Should be refactored when there is time for it. 
*/

#pragma once // Include guard

#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <fstream>
#include <cstring>
#include <future>
#include <cmath>
#include <stdio.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector3.hpp>
#include <curl/curl.h>
#include "EventQueue.hpp"
#include "ActionEvent.hpp"
#include "LogModule.hpp"
#include "BulbMath.hpp"
#include "Timer.hpp"

// Seed for the mersenne twister (random number generator)
#define SEED 31415926535897932

#define CMYINC 10	//CMYINC is divided by 100
#define LABINC 36
#define XYZINC 1
#define RGBINC 20
			
// HSV Space related defines 
#define HSVINC 3000
#define SATINC 20
#define BRIINC 20
#define MAXSAT 255
#define MAXBRI 255
#define MAXHUE 65535
#define MINBRI 1
#define MINSAT 0
#define MINHUE 0

#define DEBUG 1

class BulbHandler
{
public:
	BulbHandler();
	BulbHandler(EventQueue *eventQueue, LogModule* logger, bool* finished, bool* newGame);

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

	float calculateScore(int timeUsed);
	int* generateStartColors();
	void setBulbAdress(std::string bulbAdress);
	void setBrightness(int brightness, int bulbId);
	void setHue(int hue, int bulbId);
	void setSaturation(int saturation, int bulbId);
	void setColorSpace(ColorSpace colorSpace);
	void generateNewGoalColor();
	void setGoalColor(sf::Color color);
	void setGoalColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a=255);
	void setKValue(float k);
	void setVariables(int bulbId);
	void processEvents();
	void updateTargetBulb();
	void writeScoreAndTime(float score, int timeUsed);
	void startTimer();
	void init();
	void loadGoalColorsFromFile();
	sf::Color getGoalColor();
	static size_t callback_func(void *getInfo, size_t size, size_t count, void *stream);

	ColorSpace currentColorSpace;
	float* currentScore;
	std::vector<float> scoreVector;

private:

	bool* finished;
	bool* newGame;
	bool firstGame;
	char scoreDate[BUFFERSIZE];
	float labAlphaA;
	float labAlphaB;
	float tempScore;
	int inc = RGBINC;				//Increment used to calculate random start colors.
	int maxStartIncFromGoal = 10;	//Max number of increments between start and goal color.
	short increaseInterval;
	short playNr;

	std::stringstream message;
	std::string bulbAdress;
	sf::Color goalColor;
	sf::Vector3f values;
	CURL* curl;
	ActionEvent currentAction;
	ActionEvent lastAction;
	EventQueue *eventQueue;
	LogModule* logger;
	BulbMath mathSuite;
	Timer scoreTimer;
	Timer actionTimer;
	std::mt19937 gen;
	std::uniform_int_distribution<> rgbDistribution;
	int currentGoalColor;
	std::vector<sf::Color> colors;

	bool doesFileExist(std::string name);
	void RGBColorAdjustment(unsigned short bulbId, short inc);
	void HSVColorAdjustment(unsigned short bulbId, short inc);
	void CMYColorAdjustment(unsigned short bulbId, short inc);
	void XYZColorAdjustment(unsigned short bulbId, short inc);
	void LabColorAdjustment(unsigned short bulbId, short inc);
	void updateBulb(unsigned short bulbId, short inc);
	void command(std::string body, int bulbId);
	void writeAction(ActionEvent action, int timeUsed);
	void startNewGame();

	static sf::Vector3f Bulb1HSV;
	static sf::Vector3f Bulb2HSV;
	static sf::Vector3f Bulb3HSV;
	static sf::Vector3f Bulb4HSV;
	static bool isSetVariablesUpdated;
	static std::vector<std::string> bulbOutput;
	static float K; //the K values of the last rgb2cmyk calculation preformed
};
