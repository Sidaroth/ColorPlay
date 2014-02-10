#include <assert.h>
#include <cstdio>

#include "MoveHandler.hpp"

#define BEGIN_TEXT(x) fprintf(stderr, "Message: %s\n", x)
#define END_TEXT()    fprintf(stderr, " ... OK\n")
#define INFO(x)       fprintf(stderr, "Info: %s\n", x)


MoveHandler::MoveHandler()
{
	this->move = nullptr;
}

bool MoveHandler::connect()
{
	this->move = psmove_connect();
	if (move == nullptr) {
		printf("Could not connect to default Move controller.\n"
			"Please connect one via USB or Bluetooth.\n");
		return false;
	}

	INFO("THE MOVE CONTROLLER IS GOING TO LIGHT UP");
	BEGIN_TEXT("color test (lighting up)");
	for (int i = 0; i<255; i++) {
		psmove_set_leds(this->move, i, i, 0);
		assert(psmove_update_leds(move) == Update_Success);
	}
	END_TEXT();

	return true;
}


