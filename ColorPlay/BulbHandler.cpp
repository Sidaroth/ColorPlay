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
	increaseInterval = 250;
	rgbDistribution = std::uniform_int_distribution<>(0, 255);

	this -> logger = logger;
}

void BulbHandler::setBulbAdress(std::string bulbAdress)
{
	this -> bulbAdress = bulbAdress;
}

void BulbHandler::setBrightness(int brightness, int bulbId)
{
	message.str(std::string());
	message << "{\"on\":true, \"bri\":" << brightness << "}";

	if (DEBUG >= 1)
	{
		std::cout << "Setting brightness to: " << brightness << "\n";
	}

	command(message.str(), bulbId);
}

void BulbHandler::setHue(int hue, int bulbId)
{
	message.str(std::string());
	message << "{\"on\":true,\"hue\":" << hue << ",\"sat\":255" << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting hue to: " << hue << "\n";
	}

	command(message.str(), bulbId);
}


void BulbHandler::setSaturation(int saturation, int bulbId)
{
	message.str(std::string());
	message << "{\"on\":true, \"sat\":" << saturation << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting saturation to: " << saturation << "\n";
	}

	command(message.str(), bulbId);
}

void BulbHandler::generateNewGoalColor()
{
	unsigned int r, g, b;

	r = rgbDistribution(gen);
	g = rgbDistribution(gen);
	b = rgbDistribution(gen);

	message.str(std::string());
	message << "Generated new goal color (RGB): (" << r << ", " << g << ", " << b << ")"; 
	this -> logger -> LogEvent(message.str());
	setGoalColor(r, g, b);
}

void BulbHandler::command(std::string body, int bulbId)
{

	message.str(std::string());
	//CURLcode res;
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

		//res = curl_easy_perform(curl);
		curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}


void BulbHandler::setColorSpace(ColorSpace colorSpace)
{
	this -> currentColorSpace = colorSpace;

	message.str(std::string());
	message << "ColorSpace changed to: ";

	switch(currentColorSpace)
	{
		case ColorSpace::RGB:
			increaseInterval = RGBINC;
			message << "RGB.";
			break;

		case ColorSpace::HSV:
			increaseInterval = HSVINC;
			message << "HSV.";
			break;

		case ColorSpace::XYZ:
			increaseInterval = XYZINC;
			message << "XYZ.";
			break;

		case ColorSpace::CMY:
			increaseInterval = CMYINC;
			message << "CMY.";
			break;

		case ColorSpace::Lab:
			increaseInterval = LABINC;
			message << "Lab.";
			break;

		default:
			std::cerr << "Something went wrong in BulbHandler::setColorSpace(). Unkown ColorSpace";
			break;
	}

	logger -> LogEvent(message.str());
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

/// Depending on the event received, change the corresponding bulb (H, S, or V value)
void BulbHandler::HSVColorAdjustment(unsigned short bulbId, short inc)
{
	if(bulbId == 1)			// H
	{
		Bulb1HSV.x = Bulb1HSV.x + inc;
		Bulb2HSV.x = Bulb1HSV.x;
		Bulb3HSV.x = Bulb1HSV.x;
		message << "\"hue\": " << Bulb1HSV.x << "}";

		// Update the other lightbulbs
		command(message.str(), 2);
		command(message.str(), 3);

	}
	else if(bulbId == 2)		// S
	{
		Bulb2HSV.y = Bulb2HSV.y + inc;
		Bulb1HSV.y = Bulb2HSV.y;
		Bulb3HSV.y = Bulb2HSV.y;
		message << "\"sat\": " << Bulb2HSV.y << "}";

		command(message.str(), 1);
		command(message.str(), 3);
	}
	else if(bulbId == 3)   // V
	{
		Bulb3HSV.z = Bulb3HSV.z + inc;
		Bulb1HSV.z = Bulb3HSV.z;
		Bulb2HSV.z = Bulb3HSV.z;
		message << "\"bri\": " << Bulb3HSV.z << "}";

		command(message.str(), 1);
		command(message.str(), 2);
	}

	// Update the data on this light bulb. 
	command(message.str(), bulbId);

	// Update the data on the 4th "Target" bulb. 
	command(message.str(), 4);
}

/// Depending on the event received, change the corresponding bulb (R, G, or B value)
void BulbHandler::RGBColorAdjustment(unsigned short bulbId, short inc)
{
	if(bulbId == 1)		// R
	{
		values = mathSuite.hsv2rgb(Bulb1HSV.x, Bulb1HSV.y, Bulb1HSV.z);
		values.x = values.x + inc;
		values.y = 0;
		values.z = 0;
		Bulb1HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << Bulb1HSV.x << ", \"sat\": " << Bulb1HSV.y << ", \"bri\": " << Bulb1HSV.z << "}";
	}
	else if(bulbId == 2)		// G
	{
		values = mathSuite.hsv2rgb(Bulb2HSV.x, Bulb2HSV.y, Bulb2HSV.z);
		values.y = values.y + inc;
		values.x = 0;
		values.z = 0;

		Bulb2HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << Bulb2HSV.x << ", \"sat\": " << Bulb2HSV.y << ", \"bri\": " << Bulb2HSV.z << "}";
	}
	else if(bulbId == 3)		// B
	{
		values = mathSuite.hsv2rgb(Bulb3HSV.x, Bulb3HSV.y, Bulb3HSV.z);
		values.z = values.z + inc;
		values.x = 0;
		values.y = 0;
		Bulb3HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << Bulb3HSV.x << ", \"sat\": " << Bulb3HSV.y << ", \"bri\": " << Bulb3HSV.z << "}";
	}

	// Update the data on this light bulb. 
	command(message.str(), bulbId);

	// Update the data on the 4th "Target" bulb. 
	sf::Vector3f temp1, temp2, temp3; 
	temp1 = mathSuite.hsv2rgb(Bulb1HSV.x, Bulb1HSV.y, Bulb1HSV.z);
	temp2 = mathSuite.hsv2rgb(Bulb2HSV.x, Bulb2HSV.y, Bulb2HSV.z);
	temp3 = mathSuite.hsv2rgb(Bulb3HSV.x, Bulb3HSV.y, Bulb3HSV.z);

	values.x = Bulb1HSV.x + Bulb2HSV.x + Bulb3HSV.x;
	values.y = Bulb1HSV.y + Bulb2HSV.y + Bulb3HSV.y;
	values.z = Bulb1HSV.z + Bulb2HSV.z + Bulb3HSV.z;

	values = mathSuite.rgb2hsv(values.x, values.y, values.z);
}

/// Depending on the event received, change the corresponding bulb (C, M, or Y value)
void BulbHandler::CMYColorAdjustment(unsigned short bulbId, short inc)
{
	// nothing yet.
}


/// Depending on the event received, change the corresponding bulb (X, Y, or Z value)
void BulbHandler::XYZColorAdjustment(unsigned short bulbId, short inc)
{
	// nothing yet
}

/// Depending on the event received, change the corresponding bulb (L, a, or b value)
void BulbHandler::LabColorAdjustment(unsigned short bulbId, short inc)
{
	// nothing yet. 
}

/// Checks colorspace, and calls the corresponding function. Update the Target bulb after. 
void BulbHandler::updateBulb(unsigned short bulbId, short inc)
{
	message.str(std::string());
	message << "{\"on\":true, ";
	switch(currentColorSpace)
	{
		case ColorSpace::HSV:
			HSVColorAdjustment(bulbId, inc);
			break;

		case ColorSpace::RGB:
			RGBColorAdjustment(bulbId, inc);
			break;

		case ColorSpace::CMY:
			CMYColorAdjustment(bulbId, inc);
			break;

		case ColorSpace::XYZ:
			XYZColorAdjustment(bulbId, inc);
			break;

		case ColorSpace::Lab:
			LabColorAdjustment(bulbId, inc);
			break;

		default: break;
	}

	message.str(std::string());
	message << "{\"on\":true, " << "\"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";
	command(message.str(), 4);
}

// Process any events that have been added to the event queue. 
void BulbHandler::processEvents()
{
	unsigned short bulb = 0;
	while(!this -> eventQueue -> empty())
	{
	 	currentAction = eventQueue -> pop();

		switch(currentAction.action)
		{
			case ActionEvent::Action::Up:
				std::cout << "UP!\n";
				bulb = currentAction.getBulbID();
				updateBulb(bulb, increaseInterval);
				break;

			case ActionEvent::Action::Down:
				std::cout << "Down!\n";
				bulb = currentAction.getBulbID();
				updateBulb(bulb, increaseInterval * -1);
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

	//lol.push_back(std::string(static_cast<const char*>(getInfo), size * count));

//	std::string output = static_cast<std::string>(getInfo);

//	char **test = (char**)stream;
//	*test = strndup(getInfo, (size_t)(size* count));


	//((std::string*)stream)->append((char*)getInfo, size * count);

	//std::cout << "\nMEH-------------->" << size * count << std::endl;
	
	//std::cout << "\n HER --...->" << output << std::endl;

	found = output.find("hue");
	found2 = output.find(",", found);

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

	if (bulbIdInt == 1)
	{
		Bulb1HSV.x = hueInt;
		Bulb1HSV.y = satInt;
		Bulb1HSV.z = briInt;
		isSetVariablesUpdated = true;
	}
	else if (bulbIdInt == 2)
	{
		Bulb2HSV.x = hueInt;
		Bulb2HSV.y = satInt;
		Bulb2HSV.z = briInt;
		isSetVariablesUpdated = true;
	}
	else if(bulbIdInt == 3)
	{
		Bulb3HSV.x = hueInt;
		Bulb3HSV.y = satInt;
		Bulb3HSV.z = briInt;
		isSetVariablesUpdated = true;
	}
	else if (bulbIdInt == 4)
	{
		Bulb4HSV.x = hueInt;
		Bulb4HSV.y = hueInt;
		Bulb4HSV.z = hueInt;
		isSetVariablesUpdated = true;
	}
	else
	{
		std::cout << "\nERROR: Callback function got no ID" << std::endl;
	}
}

//updates a bulb's global variables to match the current values the bulb is showing.
void BulbHandler::setVariables(int bulbId)
{
	 //CURLcode res;
	 //CURLcode res2;
	 struct curl_slist *headers = NULL;
	 curl = curl_easy_init();
	 char* getInfo;
	 isSetVariablesUpdated = false;

	 message.str(std::string());

	 if (curl)
	 {
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

	 	//res = curl_easy_perform(curl);
	 	while(isSetVariablesUpdated == false)
	 	{
	 	curl_easy_perform(curl);
	 	}
	 	curl_slist_free_all(headers);

	 	curl_easy_cleanup(curl);
	 }

	 curl_global_cleanup();
}

