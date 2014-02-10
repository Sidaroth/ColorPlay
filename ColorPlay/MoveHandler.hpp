#pragma once

#include "psmove.h"

class MoveHandler
{
public:
	MoveHandler();
	bool connect();
private:
	PSMove *move;
};