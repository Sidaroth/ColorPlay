#pragma once

//define NDEBUG

#include <assert.h>

#include "psmove.h"

#include "LogModule.hpp"

#define BEGIN_TEXT(x) fprintf(stderr, "Message: %s\n", x)
#define END_TEXT()    fprintf(stderr, " ... OK\n")
#define INFO(x)       fprintf(stderr, "Info: %s\n", x)

class MoveHandler
{
public:
	MoveHandler(LogModule *logger);
	bool connect();
	PSMove_Connection_Type getConnectionType();
	void run();

	//Setters and getters
	void setColor(unsigned char r, unsigned char g, unsigned char b);
	char* getColor();
private:
	PSMove *move;
	LogModule *logger;
	enum PSMove_Connection_Type connectionType;
	char r,g,b;
	unsigned int buttons;

	void updateColor();
};