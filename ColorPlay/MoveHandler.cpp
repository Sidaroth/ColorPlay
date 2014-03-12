/*
	Purpose: This class is responsible for handling the communication to and from the PlayStation Move Controller.

	Last edited: 21. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gj�vik University College.
*/
#include "MoveHandler.hpp"

//PUBLIC###########################################################################################
MoveHandler::MoveHandler()
{
	#if DEBUG
		std::cout << "This MoveHandler constructor should not be called";
	#endif
}

MoveHandler::MoveHandler(	LogModule *logger,
							BulbHandler* bulbHandler,
							EventQueue * eventQueue,
							bool *running
						)
{
	this->running = running;

	this->logger = logger;

	this->connections = 0;
	this->BTController = nullptr;
	this->USBController = nullptr;
	
	this->tracker = nullptr;
	this->frame = nullptr;
	this->x = 0.0f; 
	this->y = 0.0f; 
	this->radius = 0.0f;

	this->bulbHandler = bulbHandler;
	this->eventQueue = eventQueue;
}

MoveHandler::~MoveHandler()
{

}

void MoveHandler::run()
{
	if(this->connect())
	{	
		this->timer.start();
		while(*this->running && (cvWaitKey(1) & 0xFF) != 27)
		{	
			this->updateControllers();
			this->processInputControllers();
						
			this->updateTracker();
			this->processInputTracker();
			
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		disconnect();
	}	
}

//PRIVATE##########################################################################################
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
	enum PSMove_Connection_Type connectionType;

	logger->LogEvent("Initializeing controller(s)...");
	
	this->connections = psmove_count_connected();

	if(this->connections == 0)
	{
		logger->LogEvent("No controllers connected. \nAborting!");
		return false;
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
		
		connectionType = psmove_connection_type(this->controllers[i]);
		string.str("");
		string << "Connection #" << i << " connection type: ";
		if(connectionType==Conn_Bluetooth)
		{
			string << "Bluetooth";
			this->BTController = this->controllers[i]; 
		}
		else
		{
			string << "USB";
			this->USBController = this->controllers[i];
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
		this->timer.start();
		while (*this->running && this->timer.secondsElapsed() < 60)
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
		this->bulbHandler->startTimer();
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

	psmove_tracker_get_position(this->tracker, this->USBController, &xPos, &yPos, &ledRadius);
	this->x = xPos;
	this->y = yPos;
	this->radius = ledRadius;
}

void MoveHandler::processInputControllers()
{
	ActionEvent event(0, 1);
		
	for(int i = 0; i < connections; i++)
	{	
		if(this->buttons[i] & Btn_MOVE)
		{
			if (finishedTimer.secondsElapsed() > 3)
			{
				event.setAction(ActionEvent::Action::Finish);
				finishedTimer.start();
				this->eventQueue->push(event);
				return;
			}
		}
	}
}

// Camera tracking width = 640px, 3x196px zones for bulbs, gives 52px for deadzones. 
//   0-196 -> bulb 1   - 196
// 196-222 -> deadzone - 26
// 222-418 -> bulb 2   - 196
// 418-444 -> deadzone - 26
// 444-640 -> bulb 3   - 196
void MoveHandler::processInputTracker()
{
	ActionEvent event(0, 1);
	
	if(this->timer.secondsElapsed() >= 1)
	{	
		if(this->x < 196)
		{
			event.setBulbID(1);
		}
		else if(this -> x > 222 && this -> x < 418)
		{
			event.setBulbID(2);
		}
		else if (this->x > 444)
		{
			event.setBulbID(3);
		}
		else
		{
			// Deadzone
			event.setAction(ActionEvent::Action::None); // Constructor actually sets this, but might be good for clarity?
		}

		if(this->y < 160)
		{
			event.setAction(ActionEvent::Action::Up);
		}
		else if (this->y > 320)
		{
			event.setAction(ActionEvent::Action::Down);
		}
		else
		{
			// deadzone
			event.setAction(ActionEvent::Action::None); // Constructor actually sets this, but might be good for clarity?
		}

		this->eventQueue->push(event);
		this->timer.start();
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
