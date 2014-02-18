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
	
	void run();							//Called when the thread is started. Contains main loop. 

private:
	bool *running;

	//Logger
	LogModule *logger;
	
	//PSMove
	int connections;					//Number of connectections to Move controller.
	std::vector<PSMove*> controllers;	//Vector of connectections to Move controller.
	PSMove* USBController;				//Pointer to the USB Move connection if one exists.
	PSMove* BTController;				//Pointer to the Bluetooth Move connection if one exists.
	std::vector<unsigned int> buttons;	//Buttons signaled pressed from the Move connections.

	//Tracker
	PSMoveTracker *tracker;
	void *frame;						//Camera live stream frame, used for debugging. 
	float x;							//Move controller x position in the camera frame. 
	float y;							//Move controller y position in the camera frame.
	float radius;						//Move controller LED ball radius in the camera frame.

	//Bulb interaction
	BulbHandler *bulbHandler;			//Class used to communicate with the Phillips Hue light bulbs.
	EventQueue *eventQueue;				//Queue used to organize events to the bulbHandler.
	Timer timer;						//Timer used to slowdown events to the bulbHandler.

	bool connect();						//Runs connectControllers and connectTracker.
	bool connectControllers();
	bool connectTracker();
	void updateControllers();			//Updates the button input from the controller.
	void updateTracker();				//Update the controller posistion tracking.
	void processInput();				//Process the input form the buttons and the tracker.
	void disconnect();					//Disconnects from the tracker and Move controller.
};