#pragma once

//#include <sstream>

#include "psmove.h"
#include "psmove_tracker.h"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"

#include "struct.hpp"
#include "LogModule.hpp"

#define MEASUREMENTS 50

class MoveHandler
{
public:
	MoveHandler(LogModule *logger, bool *running);
	bool connect();
	void disconnect();
	void run();
	void run2();

	//Setters and getters
	void setColor(unsigned char r, unsigned char g, unsigned char b);
private:
	bool *running;

	//PSMove
	PSMove *move;
	enum PSMove_Connection_Type connectionType;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned int buttons;

	//Tracker
	PSMoveTracker *tracker;
	void *frame;
	float *x; 
	float *y; 
	float *radius;
	measurement *measurements;

	LogModule *logger;



	void updateColor();
};