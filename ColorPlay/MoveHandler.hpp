/*
	Purpose: This class is responsible for handling the communication to and from the PlayStation Move Controller.

	Last edited: 18. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gj�vik University College.
*/
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
	int connections;								//Number of connectections to Move controller.
	std::vector<PSMove*> controllers;				//Vector of connectections to Move controller.
	PSMove* USBController;
	PSMove* BTController;
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