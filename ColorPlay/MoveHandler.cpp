#include "MoveHandler.hpp"

//PUBLIC###########################################################################################
MoveHandler::MoveHandler(LogModule *logger, bool *running)
{
	this->running = running;

	this->move = nullptr;
	this->tracker = nullptr;
	this->measurements = new measurement[MEASUREMENTS];
	this->logger = logger;
	
	r = 0;
	g = 0;
	b = 0;
	buttons = 0;
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

	if(connectionType==Conn_Bluetooth)
	{
		logger->LogEvent("Connection type: Bluetooth");
	}
	else
	{
		logger->LogEvent("Connection type: USB");
	}

	this->tracker = psmove_tracker_new();

	if(this->tracker == nullptr || this->tracker == NULL)
	{
		logger->LogEvent("Could not create tracker.");
		return false;
	}
	
	logger->LogEvent("Calibrating controller...");
	while (psmove_tracker_enable(this->tracker, this->move) != Tracker_CALIBRATED);
	logger->LogEvent("Calibration finished.");


	return true;
}

void MoveHandler::run()
{
	this->connect();
	
	while(this->running)
	{	
		psmove_poll(this->move);
		this->updateColor();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
		
}

void MoveHandler::run2()
{
	this->connect();
	this->setColor(255, 0, 0);

	while(this->connectionType == Conn_Bluetooth && !(buttons & Btn_MOVE))
	{	
		if(psmove_poll(this->move))
		{
			buttons = psmove_get_buttons(this->move);

			if(buttons & Btn_CIRCLE)
			{
				this->r = 255;
			}
			else
			{
				this->r = 0;
			}

			if(buttons & Btn_TRIANGLE)
			{
				this->g =255;
			}
			else
			{
				this->g = 0;
			}

			if(buttons & Btn_CROSS)
			{
				this->b = 255;
			}
			else
			{
				this->b = 0;
			}

		}

		this->updateColor();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	this->setColor(0, 0, 0);
	this->running = false;
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
	//std::stringstream string;
	//string << "Updating Color: " << (int)this->r << ", " << (int)this->g << ", " << (int)this->b;
	//std::cout << string.str() << std::endl;
	
	psmove_set_leds(this->move, this->r, this->g, this->b);
	PSMove_Update_Result result = psmove_update_leds(this->move);
	if(result == Update_Failed)
	{
		std::cout << "LED update failed\n";		
	}
	else if (result == Update_Ignored)
	{
		//std::cout << "LED update ignored\n";
	}
	else if(result == Update_Success)
	{
		std::cout << "LED update succeeded\n";
	}
	else
	{
		std::cout << "WAT?!\n";
	}
}
