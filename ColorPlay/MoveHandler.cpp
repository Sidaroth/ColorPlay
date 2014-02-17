#include "MoveHandler.hpp"

//PUBLIC###########################################################################################
MoveHandler::MoveHandler(LogModule *logger, BulbHandler* bulbHandler, bool *running)
{
	this->logger = logger;
	this->bulbHandler = bulbHandler;

	this->running = running;

	this->connections = 0;
	r = 0;
	g = 0;
	b = 0;
	
	this->tracker = nullptr;
	this->frame = nullptr;
}

void MoveHandler::run()
{
	if(this->connect())
	{	
		while(this->running && (cvWaitKey(1) & 0xFF) != 27)
		{	
			this->updateControllers();			
			this->updateTracker();

			this->processInput();

			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		disconnect();
	}	
}

bool MoveHandler::connect()
{
	if(!this->connectControllers())
	{
		return false;
	}

	if(!this->connectTracker())
	{
		return false;
	}

	return true;
}

bool MoveHandler::connectControllers()
{
	std::stringstream string;

	logger->LogEvent("Initializeing controller(s)...");
	
	this->connections = psmove_count_connected();

	if(this->connections == 0)
	{
		logger->LogEvent("No controllers connected. \nAborting!");
	}
	
	for(int i = 0; i < this->connections; i++)
	{
		PSMove* connection = nullptr;
		unsigned int buttons = 0;

		this->controllers.push_back(connection);
		this->buttons.push_back(buttons);
	}

	string << "Move connections: " << this->connections;
	logger->LogEvent(string.str());

	for(int i = 0; i < this->connections; i++)
	{
		string.str("");
		string << "Connecting to connection #" << i;
		logger->LogEvent(string.str());
		
		this->controllers[i] = psmove_connect_by_id(i);

		string.str("");
		if(this->controllers[i] == nullptr || this->controllers[i] == NULL)
		{
			string << "Could not connect to connection #" << i << "\nAborting!";		
			return false;
		}
		else
		{
			string << "Connected to connection #" << i;
		}
		logger->LogEvent(string.str());
		
		this->connectionType = psmove_connection_type(this->controllers[i]);
		string.str("");
		string << "Connection #" << i << " connection type: ";
		if(this->connectionType==Conn_Bluetooth)
		{
			string << "Bluetooth";
		}
		else
		{
			string << "USB";
		}	
		logger->LogEvent(string.str());
	}

    logger->LogEvent("Controller initialization finished");
    return true;
}

bool MoveHandler::connectTracker()
{
	std::stringstream string;

	logger->LogEvent("Initializeing tracker...");
	tracker = psmove_tracker_new();
	if(this->tracker == nullptr || this->tracker == NULL)
	{
		logger->LogEvent("Could not create tracker.\nAborting!");
		return false;
	}
	logger->LogEvent("OK");

	for(int i = 0; i < this->connections; i++)
	{	
		while (this->running)
		{
			string.str("");
			string << "Calibrating connection #" << i;
			logger->LogEvent(string.str());
		

            if (psmove_tracker_enable(this->tracker, this->controllers[i]) == Tracker_CALIBRATED) 
            {
        		string.str("");
				string << "Enable automatic LED update for connection #" << i;
				logger->LogEvent(string.str());
				psmove_tracker_set_auto_update_leds(this->tracker, this->controllers[i], PSMove_True);
				
				break;
            }
            else
            {
				logger->LogEvent("Caibration failed. Retrying.");	
            }
        }

		logger->LogEvent("Calibration finished.");
	}

	logger->LogEvent("Enable tracker mirroring");
    psmove_tracker_set_mirror(this->tracker, PSMove_True);

	logger->LogEvent("Controller initialization finished");
	return true;
}

void MoveHandler::updateControllers()
{
	for(int i = 0; i < connections; i++)
	{
		psmove_poll(this->controllers[i]);
		this->buttons[i] = psmove_get_buttons(this->controllers[i]);
	}
}

void MoveHandler::updateTracker()
{
	float xPos = 0.0f; 
	float yPos = 0.0f; 
	float ledRadius = 0.0f;

	psmove_tracker_update_image(this->tracker);
	psmove_tracker_update(this->tracker, NULL);

	#if DEBUG		
		psmove_tracker_annotate(this->tracker);
		frame = psmove_tracker_get_frame(this->tracker);
		if (frame)
		{
			cvShowImage("Live camera feed", frame);
		}
	#endif

	psmove_tracker_get_position(this->tracker, this->controllers[0], &xPos, &yPos, &ledRadius);
	this->x = xPos;
	this->y = yPos;
	this->radius = ledRadius;

	#if DEBUG			
		printf("\nx: %10.2f, y: %10.2f, r: %10.2f\n", this->x, this->y, this->radius);
	#endif
}

void MoveHandler::processInput()
{
	int bulb = 1;

	for(int i = 0; i < connections; i++)
	{
		if(this->buttons[i] & Btn_MOVE)
		{
			this->running = false;
		}
	}
	
	if(this->x <200)
	{
		bulb = 1;
	}
	else if (this->x > 400)
	{
		bulb = 3;
	}
	else
	{
		bulb = 2;
	}

	if(this->y < 200)
	{
		this->bulbHandler->setHue(20000, bulb);
	}
	else if (this->y > 400)
	{	
		this->bulbHandler->setHue(60000, bulb);	
	}
	else
	{
		this->bulbHandler->setHue(40000, bulb);		
	}

}

void MoveHandler::disconnect()
{
	logger->LogEvent("Disconnecting controller.");
	
	cvDestroyAllWindows();
	this->frame = nullptr;
	for(int i = 0; i < this->connections; i++)
	{
		psmove_disconnect(this->controllers[i]);
	}
	psmove_tracker_free(this->tracker);
}

