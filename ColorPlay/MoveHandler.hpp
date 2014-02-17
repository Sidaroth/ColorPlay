#pragma once

#include <sstream>

#include "psmove.h"
#include "psmove_tracker.h"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"

#include "struct.hpp"
#include "LogModule.hpp"
#include "BulbHandler.hpp"

#define DEBUG 1

class MoveHandler
{
public:
	MoveHandler(LogModule *logger, BulbHandler* bulbHandler, bool *running);
	void run();

private:
	bool *running;

	//PSMove
	int connections;
	std::vector<PSMove*> controllers;
	enum PSMove_Connection_Type connectionType;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	std::vector<unsigned int> buttons;

	//Tracker
	PSMoveTracker *tracker;
	void *frame;
	float x = 0.0f; 
	float y = 0.0f; 
	float radius = 0.0f;

	LogModule *logger;
	BulbHandler *bulbHandler;

	bool connect();
	bool connectControllers();
	bool connectTracker();
	void updateControllers();
	void updateTracker();
	void processInput();
	void disconnect();
};