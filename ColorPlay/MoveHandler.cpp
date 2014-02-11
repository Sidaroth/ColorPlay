#include "MoveHandler.hpp"

//PUBLIC###########################################################################################
MoveHandler::MoveHandler(LogModule *logger)
{
	this->move = nullptr;
	this->logger = logger;
	r = 0;
	g = 0;
	b = 0;
}

bool MoveHandler::connect()
{
	this->move = psmove_connect();

	if(move == nullptr || move == NULL)
	{
		logger->LogEvent("Could not connect to default Move controller.\nPlease connect one via USB or Bluetooth.\n");
		return false;
	}
	
	connectionType = psmove_connection_type(this->move);
	logger->LogEvent("Default move controller connected.");
	return true;
}

PSMove_Connection_Type MoveHandler::getConnectionType()
{
	return this->connectionType;
}

void MoveHandler::run()
{
	this->connect();
	this->setColor(255,255,255);

	while(getConnectionType() == Conn_Bluetooth && !(buttons & Btn_MOVE))
	{
		if(psmove_poll(this->move))
		{
			buttons  = psmove_get_buttons(this->move);

			if(buttons & Btn_CIRCLE)
			{
				this->r = 255;
				this->logger->LogEvent("MoveHandler: Btn_CIRCLE");
			}
			else
			{
				this->r = 0;
			}

			if(buttons & Btn_TRIANGLE)
			{
				this->g = 255;
				this->logger->LogEvent("MoveHandler: Btn_TRIANGLE");
			}
			else
			{
				this->g = 0;
			}

			if(buttons & Btn_CROSS)
			{
				this->b = 255;
				this->logger->LogEvent("MoveHandler: Btn_CROSS");
			}
			else
			{
				this->b = 0;
			}
		}
		this->updateColor();
	}
	this->setColor(0, 0, 0);
}

void MoveHandler::setColor(unsigned char r, unsigned char g, unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;

	this->updateColor();
}

char* MoveHandler::getColor()
{
	char* color = new char[this->r, this->g, this->b];
	return color;
}

//PRIVATE##########################################################################################
void MoveHandler::updateColor()
{
	psmove_set_leds(this->move, this->r, this->g, this->b);
	psmove_update_leds(this->move);
}
