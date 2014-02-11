#include "MoveHandler.hpp"

//PUBLIC###########################################################################################
MoveHandler::MoveHandler(LogModule *logger)
{
	this->move = nullptr;
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
	return true;
}

PSMove_Connection_Type MoveHandler::getConnectionType()
{
	return this->connectionType;
}

void MoveHandler::run()
{
	//std::cout << "MoveHandler::run: Start" << std::endl;
	this->connect();
	this->setColor(255,255,255);
	//psmove_set_rate_limiting(this->move, PSMove_False);

	// if(getConnectionType() == Conn_Unknown)
	// {
	// 	std::cout << "MoveHandler::run: Connection type unknown." << std::endl;
	// }
	// else if (getConnectionType() == Conn_Bluetooth)
	// {
	// 	std::cout << "MoveHandler::run: Connection type Bluetooth." << std::endl;
	// }
	// else if (getConnectionType() == Conn_USB)
	// {
	// 	std::cout << "MoveHandler::run: Connection type USB." << std::endl;
	// }
	// else
	// {
	// 	std::cout << "MoveHandler::run: Connection type WAT?!." << std::endl;
	// }

	while(getConnectionType() == Conn_Bluetooth && !(buttons & Btn_MOVE))
	{	
		//std::cout << "MoveHandler::run: While start" << std::endl;
		if(psmove_poll(this->move))
		{
			buttons = psmove_get_buttons(this->move);

			if(buttons & Btn_CIRCLE)
			{
				this->r = 82;
				//std::cout << "MoveHandler: Btn_CIRCLE" << std::endl;
			}
			else
			{
				this->r = 0;
			}

			if(buttons & Btn_TRIANGLE)
			{
				this->g = 71;
				//std::cout << "MoveHandler: Btn_TRIANGLE" << std::endl;
			}
			else
			{
				this->g = 0;
			}

			if(buttons & Btn_CROSS)
			{
				this->b = 66;
				//std::cout << "MoveHandler: Btn_CROSS" << std::endl;
			}
			else
			{
				this->b = 0;
			}

		}

		this->updateColor();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		//std::cout << "MoveHandler::run: While End" << std::endl;
	}
	this->setColor(0, 0, 0);
	//std::cout << "MoveHandler::run: End" << std::endl;
}

void MoveHandler::setColor(unsigned char r, unsigned char g, unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;

	//std::stringstream string;
	//string << "Setting Color: " << (int)this->r << ", " << (int)this->g << ", " << (int)this->b;
	//std::cout << string.str() << std::endl;			
	
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
	std::stringstream string;
	string << "Updating Color: " << (int)this->r << ", " << (int)this->g << ", " << (int)this->b;
	std::cout << string.str() << std::endl;
	
	psmove_set_leds(this->move, this->r, this->g, this->b);
	psmove_update_leds(this->move);
}
