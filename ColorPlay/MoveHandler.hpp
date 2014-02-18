#pragma once

#include <sstream>

#include "psmove.h"
#include "psmove_tracker.h"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"

#include "struct.hpp"
#include "LogModule.hpp"
#include "BulbHandler.hpp"
#include "EventQueue.hpp"
#include "ActionEvent.hpp"
#include "Timer.hpp"

#define DEBUG 1

class MoveHandler
{
public:
	MoveHandler();
	MoveHandler(LogModule *logger,
				BulbHandler *bulbHandler,
				EventQueue * eventQueue,
				bool *running);
	~MoveHandler();
	
	void run();

private:
	bool *running;

	//Logger
	LogModule *logger;
	
	//PSMove
	int connections;
	std::vector<PSMove*> controllers;
	PSMove* USBController;
	PSMove* BTController;
	enum PSMove_Connection_Type connectionType;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	std::vector<unsigned int> buttons;

	//Tracker
	PSMoveTracker *tracker;
	void *frame;
	float x; 
	float y; 
	float radius;

	//Bulb interaction
	BulbHandler *bulbHandler;
	EventQueue *eventQueue;
	Timer timer;

	bool connect();
	bool connectControllers();
	bool connectTracker();
	void updateControllers();
	void updateTracker();
	void processInput();
	void disconnect();
};