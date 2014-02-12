#pragma once

//define NDEBUG

#include <assert.h>
#include <sstream>
//#include <mutex>

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
	void run();
	void run2();

	//Setters and getters
	void setColor(unsigned char r, unsigned char g, unsigned char b);
	char* getColor();
private:
	bool *running;

	PSMove *move;
	enum PSMove_Connection_Type connectionType;
	unsigned char r,g,b;
	unsigned int buttons;

	PSMoveTracker *tracker;
	measurement *measurements;

	LogModule *logger;



	void updateColor();
};