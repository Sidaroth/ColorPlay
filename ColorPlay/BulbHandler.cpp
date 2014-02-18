#include "BulbHandler.hpp"
#include <functional>

BulbHandler::BulbHandler()
{
	curl = curl_easy_init();
	currentColorSpace = ColorSpace::HSV;
	goalColor = sf::Color(255, 255, 255);

	gen = std::mt19937(SEED);
	rgbDistribution = std::uniform_int_distribution<>(0, 255);
}

BulbHandler::BulbHandler(EventQueue *eventQueue, LogModule* logger)
{
	curl = curl_easy_init();
	currentColorSpace = ColorSpace::HSV;
	
	this -> eventQueue = eventQueue;
	gen = std::mt19937(SEED);
	rgbDistribution = std::uniform_int_distribution<>(0, 255);

	this -> logger = logger;
}

void BulbHandler::setBulbAdress(std::string bulbAdress)
{
	this -> bulbAdress = bulbAdress;
}

void BulbHandler::setBrightness(int brightness, int bulbId)
{
	std::stringstream message;
	message << "{\"on\":true, \"bri\":" << brightness << "}";

	if (DEBUG >= 1)
	{
		std::cout << "Setting brightness to: " << brightness << "\n";
	}

	command(message.str(), bulbId);
}

int BulbHandler::getBrightness(int bulbId)
{
	return 0;
}

void BulbHandler::setHue(int hue, int bulbId)
{
	std::stringstream message;
	message << "{\"on\":true,\"hue\":" << hue << ",\"sat\":255" << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting hue to: " << hue << "\n";
	}

	command(message.str(), bulbId);
}

//All the get functions do not work for now. Use setVariables(bulbId) then copy the vaules from the global Vector3f object
int BulbHandler::getHue(int bulbId)
{
	std::stringstream message;

	//commandGet(bulbId);

	return 0;
}


void BulbHandler::setSaturation(int saturation, int bulbId)
{
	std::stringstream message;
	message << "{\"on\":true, \"sat\":" << saturation << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting saturation to: " << saturation << "\n";
	}

	command(message.str(), bulbId);
}

int BulbHandler::getSaturation(int bulbId)
{
	return 0;
}


void BulbHandler::generateNewGoalColor()
{
	unsigned int r, g, b;

	r = rgbDistribution(gen);
	g = rgbDistribution(gen);
	b = rgbDistribution(gen);

	std::stringstream message;
	message << "Generated new goal color (RGB): (" << r << ", " << g << ", " << b << ")"; 
	this -> logger -> LogEvent(message.str());
	setGoalColor(r, g, b);
}

void BulbHandler::command(std::string body, int bulbId)
{

	std::stringstream message; 
	CURLcode res;
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();

	if (curl)
	{
		if (DEBUG >= 1)
		{
			printf("Sending request \n");
		}

		//Remove data from stdout
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);
	
		// Set headers. 
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		message << bulbAdress << bulbId << "/state";

		if (DEBUG >= 1)
		{
			std::cout << "Received body is: " << &body[0] << "\n";
			std::cout << "Bulb adress: " << &message.str()[0] << "\n";
		}

		curl_easy_setopt(curl, CURLOPT_URL, &message.str()[0]);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &body[0]);

		res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}


void BulbHandler::setColorSpace(ColorSpace colorSpace)
{
	this -> currentColorSpace = colorSpace;
}

void BulbHandler::setGoalColor(sf::Color color)
{
	this -> goalColor = color;
}

void BulbHandler::setGoalColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a /* 255 */)
{
	this -> goalColor = sf::Color(r, g, b, a);
}

sf::Color BulbHandler::getGoalColor()
{
	return this -> goalColor;
}

// Process any events that have been added to the event queue. 
void BulbHandler::processEvents()
{
	while(!this -> eventQueue -> empty())
	{
	 	currentAction = eventQueue -> pop();

		switch(currentAction.action)
		{
			case ActionEvent::Action::Up:
				std::cout << "UP!\n";
				break;

			case ActionEvent::Action::Down:
				std::cout << "Down!\n";
				break;

			case ActionEvent::Action::Finish:
				std::cout << "Finish!\n";
				break;

			case ActionEvent::Action::None:
				std::cout << "None! - Should not be called unless an event has been mistakingly created!\n";
				break;

			default: break;
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}


void callback_func(void *getInfo, size_t size, size_t count, void *stream)
{
	int hueInt, briInt, satInt, bulbIdInt, found, found2;
	std::string hue, sat, bri;
	std::string bulbId;
	std::string output((char*)getInfo);

	std::cout << "\n HER --...->" << output << std::endl;

	found = output.find("hue");
	found2 = output.find(",", found);

	std::cout << "\nFound: " << found << " " << found2 << std::endl;

	hue = output.substr(found + 5, found2 - found - 5);

	found = output.find("Hue Lamp");
	found2 = output.find(",", found);

	bulbId = output.substr(found + 9, 1);

	found = output.find("sat");
	found2 = output.find(",", found);

	sat = output.substr(found + 5, found2 - found - 5);

	found = output.find("bri");
	found2 = output.find(",", found);

	bri = output.substr(found + 5, found2 - found - 5);

	hueInt = atoi(hue.c_str());
	bulbIdInt = atoi(bulbId.c_str());
	satInt = atoi(sat.c_str());
	briInt = atoi(bri.c_str());

	std::cout << "------>> " << hueInt << "------->> " << bulbIdInt << "------->> " << satInt << "------>> " << briInt << std::endl;

	if (bulbIdInt == 1)
	{
		Bulb1HSV.x = hueInt;
		Bulb1HSV.y = satInt;
		Bulb1HSV.z = briInt;
	}
	else if (bulbIdInt == 2)
	{
		Bulb2HSV.x = hueInt;
		Bulb2HSV.y = satInt;
		Bulb2HSV.z = briInt;
	}
	else if(bulbIdInt == 3)
	{
		Bulb3HSV.x = hueInt;
		Bulb3HSV.y = satInt;
		Bulb3HSV.z = briInt;
	}
	else
	{
		std::cout << "\nERROR: Callback function got no ID";
	}
}

//updates a bulb's global variables to match the current values the bulb is showing.
void BulbHandler::setVariables(int bulbId)
{
	 CURLcode res;
	 CURLcode res2;
	 struct curl_slist *headers = NULL;
	 curl = curl_easy_init();
	 char* getInfo;

	 std::stringstream message;

	 if (curl)
	 {
	 	if (DEBUG >= 1)
	 	{
	 		printf("Sending request \n");
	 	}

	 	// Set headers. 
	 	headers = curl_slist_append(headers, "Accept: application/json");
	 	headers = curl_slist_append(headers, "Content-Type: application/json");
	 	headers = curl_slist_append(headers, "charsets. utf-8");
	 	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		message << bulbAdress << bulbId;

	 	curl_easy_setopt(curl, CURLOPT_URL, &message.str()[0]);
	 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

	 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_func);


	 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getInfo);

	 	res = curl_easy_perform(curl);

	 	curl_slist_free_all(headers);

	 	curl_easy_cleanup(curl);
	 }

	 curl_global_cleanup();
}

