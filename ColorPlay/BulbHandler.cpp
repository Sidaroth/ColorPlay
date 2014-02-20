/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 
			 as well as controlling the system concerned with distuinguishing the colorspaces and lightbulb cooperation. 
	
	Last edited: 20. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun, Harry Nystad
			 Gj�vik University College.
*/

#include "BulbHandler.hpp"
#include <functional>


sf::Vector3f BulbHandler::Bulb1HSV(0, 0, 0);
sf::Vector3f BulbHandler::Bulb2HSV(0, 0, 0);
sf::Vector3f BulbHandler::Bulb3HSV(0, 0, 0);
sf::Vector3f BulbHandler::Bulb4HSV(0, 0, 0);
bool BulbHandler::isSetVariablesUpdated = false;
std::vector<std::string> BulbHandler::bulbOutput;


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
	increaseInterval = HSVINC;
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

int* BulbHandler::generateStartColors()
{
	int *colors = new int[3];
	std::uniform_int_distribution<> randGend = std::uniform_int_distribution<>(-this->inc * this->maxStartIncFromGoal, this->inc * this->maxStartIncFromGoal);


	colors[0] = goalColor.r + randGend(gen);
	colors[1] = goalColor.g + randGend(gen);
	colors[2] = goalColor.b + randGend(gen);

	for(int i = 0; i < 3; i++)
	{
		if (colors[i] < 0)
		{
			colors[i] = 0;
		}
		else if (255 < colors[i])
		{
			colors[i] = 255;
		}
	}

	return colors;
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

	int* colors = generateStartColors();

	switch(currentColorSpace)
	{
		case ColorSpace::RGB:
			increaseInterval = RGBINC;
			generateNewGoalColor();
			BulbHandler::Bulb1HSV = sf::Vector3f(colors[0], 0, 0);
			BulbHandler::Bulb2HSV = sf::Vector3f(0, colors[1], 0);
			BulbHandler::Bulb3HSV = sf::Vector3f(0, 0, colors[2]);
			message << "RGB.";
			break;

		case ColorSpace::HSV:
			increaseInterval = HSVINC;
			generateNewGoalColor();
			BulbHandler::Bulb1HSV = mathSuite.rgb2hsv(colors[0], colors[1], colors[2]);
			BulbHandler::Bulb2HSV = BulbHandler::Bulb1HSV;
			BulbHandler::Bulb3HSV = BulbHandler::Bulb1HSV;
			message << "HSV.";
			break;

		case ColorSpace::XYZ:
			increaseInterval = XYZINC;
			generateNewGoalColor();
			message << "XYZ.";
			break;

		case ColorSpace::CMY:
			increaseInterval = CMYINC;
			generateNewGoalColor();
			message << "CMY.";
			break;

		case ColorSpace::Lab:
			increaseInterval = LABINC;
			generateNewGoalColor();
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

/// Update the data on the 4th "Target" bulb. 
void BulbHandler::updateTargetBulb()
{

}

/// Depending on the event received, change the corresponding bulb (H, S, or V value)
void BulbHandler::HSVColorAdjustment(unsigned short bulbId, short inc)
{
	std::string helper;
	int val; 
	if(bulbId == 1)			// H
	{
		val = BulbHandler::Bulb1HSV.x + inc;
		if(val > MAXHUE)
		{
			val = MAXHUE;
		}
		else if(val < MINHUE)
		{
			val = MINHUE;
		}

		BulbHandler::Bulb1HSV.x = val;
		BulbHandler::Bulb2HSV.x = BulbHandler::Bulb1HSV.x;
		BulbHandler::Bulb3HSV.x = BulbHandler::Bulb1HSV.x;
		message << "\"hue\": " << BulbHandler::Bulb1HSV.x << "}";

		// Update the other lightbulbs

		helper = message.str();
		command(helper, 2);
		command(helper, 3);

	}
	else if(bulbId == 2)		// S
	{
		val = BulbHandler::Bulb2HSV.y + inc;
		if(val > MAXSAT)
		{
			val = MAXSAT;
		}
		else if(val < MINSAT)
		{
			val = MINSAT;
		}

		BulbHandler::Bulb2HSV.y = val;
		BulbHandler::Bulb1HSV.y = BulbHandler::Bulb2HSV.y;
		BulbHandler::Bulb3HSV.y = BulbHandler::Bulb2HSV.y;
		message << "\"sat\": " << BulbHandler::Bulb2HSV.y << "}";

		helper = message.str();
		command(helper, 1);
		command(helper, 3);	

	}
	else if(bulbId == 3)   // V
	{
		val = BulbHandler::Bulb3HSV.z + inc;
		if(val > MAXBRI)
		{
			val = MAXBRI;
		}
		else if(val < MINBRI)
		{
			val = MINBRI;	
		}

		BulbHandler::Bulb3HSV.z = val;
		BulbHandler::Bulb1HSV.z = BulbHandler::Bulb3HSV.z;
		BulbHandler::Bulb2HSV.z = BulbHandler::Bulb3HSV.z;
		message << "\"bri\": " << Bulb3HSV.z << "}";

		helper = message.str();
		command(helper, 1);
		command(helper, 2);	
	}

	// Update the data on this light bulb. 
	command(helper, bulbId);
	command(helper, 4);
}

/// Depending on the event received, change the corresponding bulb (R, G, or B value)
void BulbHandler::RGBColorAdjustment(unsigned short bulbId, short inc)
{
	if(bulbId == 1)		// R
	{
		values = mathSuite.hsv2rgb(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
		values.x = values.x + inc;
		values.y = 0;
		values.z = 0;
		BulbHandler::Bulb1HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << BulbHandler::Bulb1HSV.x << ", \"sat\": " << BulbHandler::Bulb1HSV.y << ", \"bri\": " << BulbHandler::Bulb1HSV.z << "}";
	}
	else if(bulbId == 2)		// G
	{
		values = mathSuite.hsv2rgb(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
		values.y = values.y + inc;
		values.x = 0;
		values.z = 0;

		BulbHandler::Bulb2HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << BulbHandler::Bulb2HSV.x << ", \"sat\": " << BulbHandler::Bulb2HSV.y << ", \"bri\": " << BulbHandler::Bulb2HSV.z << "}";
	}
	else if(bulbId == 3)		// B
	{
		values = mathSuite.hsv2rgb(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);
		values.z = values.z + inc;
		values.x = 0;
		values.y = 0;
		BulbHandler::Bulb3HSV = mathSuite.rgb2hsv(values.x, values.y, values.z);

		message  << "\"hue\": " << BulbHandler::Bulb3HSV.x << ", \"sat\": " << BulbHandler::Bulb3HSV.y << ", \"bri\": " << BulbHandler::Bulb3HSV.z << "}";
	}

	// Update the data on this light bulb. 
	command(message.str(), bulbId);

	// Bulb 4 / target bulb updating for RGB. 
	sf::Vector3f temp1, temp2, temp3; 
	temp1 = mathSuite.hsv2rgb(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
	temp2 = mathSuite.hsv2rgb(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
	temp3 = mathSuite.hsv2rgb(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);

	values.x = temp1.x + temp2.x + temp3.x;
	values.y = temp1.y + temp2.y + temp3.y;
	values.z = temp1.z + temp2.z + temp3.z;

	values = mathSuite.rgb2hsv(values.x, values.y, values.z);

	//Sending values to bulb 4 /target
	message.str(std::string());
	message << "{\"on\":true, " << "\"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";
	command(message.str(), 4);
}

/// Depending on the event received, change the corresponding bulb (C, M, or Y value)
void BulbHandler::CMYColorAdjustment(unsigned short bulbId, short inc)
{
	// float* values;
	
	// if(bulbId == 1)		// C
	// {
	// 	values = mathSuite.hsv2cmyk(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
	// 	values[0] = values[0] + inc;
	// 	BulbHandler::Bulb1HSV = mathSuite.cmyk2hsv(values[0], values[1], values[2], values[3]);
	// }
	// else if(bulbId == 2) // M
	// {
	// 	values = mathSuite.hsv2cmyk(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
	// 	values[1] = values[1] + inc;
	// 	BulbHandler::Bulb2HSV = mathSuite.cmyk2hsv(values[0], values[1], values[2], values[3]);
	// }
	// else if(bulbId == 3) // Y
	// {
	// 	values = mathSuite.hsv2cmyk(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);
	// 	values[2] = values[2] + inc;
	// 	BulbHandler::Bulb3HSV = mathSuite.cmyk2hsv(values[0], values[1], values[2], values[3]);
	// }

	// // Update the data on this light bulb. 
	// command(message.str(), bulbId);

	// // Bulb 4 / target bulb updating for RGB. 
	// float* temp1, temp2, temp3;
	// temp1 = mathSuite.hsv2rgb(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
	// temp2 = mathSuite.hsv2rgb(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
	// temp3 = mathSuite.hsv2rgb(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);

	// values.x = temp1.x + temp2.x + temp3.x;
	// values.y = temp1.y + temp2.y + temp3.y;
	// values.z = temp1.z + temp2.z + temp3.z;

	// values = mathSuite.rgb2hsv(values.x, values.y, values.z);
	// command(message.str(), 4);
}

/// Depending on the event received, change the corresponding bulb (X, Y, or Z value)
void BulbHandler::XYZColorAdjustment(unsigned short bulbId, short inc)
{
	if(bulbId == 1) // X
	{
		values = mathSuite.hsv2xyz(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
		values.x = values.x + inc;
	}
	else if(bulbId == 2) // Y
	{
		values = mathSuite.hsv2xyz(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
		values.y = values.y + inc;
	}
	else if(bulbId == 3) // Z
	{
		values = mathSuite.hsv2xyz(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);
		values.z = values.z + inc;
	}
}


/// Depending on the event received, change the corresponding bulb (L, a, or b value)
void BulbHandler::LabColorAdjustment(unsigned short bulbId, short inc)
{
	if(bulbId == 1) // L
	{

	}
	else if(bulbId == 2) // a
	{

	}
	else if(bulbId == 3) // b
	{

	}
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

//this needs more testing to check date change logging
void BulbHandler::writeScore(float score)
{
	std::sort(scoreVector.begin(), scoreVector.end());
	std::reverse(scoreVector.begin(), scoreVector.end());

	std::chrono::time_point<std::chrono::system_clock> now;
	std::stringstream date;
	char buffer[BUFFERSIZE];
	struct tm * timeinfo;
	std::time_t logTime;

	now = std::chrono::system_clock::now();
	logTime = std::chrono::system_clock::to_time_t(now);

	time(&logTime);
	timeinfo = localtime(&logTime);
	strftime(buffer, BUFFERSIZE, "%F",timeinfo);

	if (!std::strcmp(buffer, this -> scoreDate))
	{
		//std::cout << "\n JAAAAAAAAAAAAAA" << std::endl;
	}
	else
	{
		//std::cout << "\n NOOOOOOOOOOOOO" << std::endl;
		std::strcpy(this -> scoreDate, buffer);
		scoreVector.clear();
	}

	date << buffer << ".txt";

	scoreVector.push_back(score);

	std::ofstream scoreFile(date.str());

	for (int i = 0; i < scoreVector.size(); i++)
	{
		scoreFile << scoreVector[i] << std::endl;
	}

	scoreFile.close();
}

//Calculates the score based on how far from the goal color the current color in bulb 4 is. All scoring is done in RGB colorSpace for simplicity
//calculateScore should be called when the player signals that they are done mixing a color
float BulbHandler::calculateScore()
{
	float score;
	sf::Vector3f scoreVector;
	sf::Color goalColor = getGoalColor();

//	values = mathSuite.hsv2rgb(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);

	if (this -> currentColorSpace == ColorSpace::RGB)
	{

	}
	else if (this -> currentColorSpace == ColorSpace::HSV)
	{
		scoreVector = mathSuite.hsv2rgb(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
	}
	else if (this -> currentColorSpace == ColorSpace::XYZ)
	{
		scoreVector = mathSuite.xyz2rgb(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
	}
	else if (this -> currentColorSpace == ColorSpace::Lab)
	{
		scoreVector = mathSuite.lab2xyz(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		scoreVector = mathSuite.xyz2rgb(scoreVector.x, scoreVector.y, scoreVector.z);
	}
	else if (this -> currentColorSpace == ColorSpace::CMY)
	{
		scoreVector = mathSuite.cmyk2rgb(1.0f, BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
	}
	else
	{
		std::cout << "\nERROR from calculate score: No Colorspace defined in currentColorSpace" << std::endl;
	}

	//(scoreVector.x >= goalColor.r) ? (scoreVector.x = scoreVector.x - goalColor.r) : (scoreVector.x = goalColor.r - scoreVector.x);
	//(scoreVector.y >= goalColor.g) ? (scoreVector.y = scoreVector.y - goalColor.g) : (scoreVector.y = goalColor.g - scoreVector.y);
	//(scoreVector.z >= goalColor.b) ? (scoreVector.z = scoreVector.z - goalColor.b) : (scoreVector.z = goalColor.b - scoreVector.z);

	scoreVector.x = (scoreVector.x >= goalColor.r) ? (scoreVector.x - goalColor.r) : (goalColor.r - scoreVector.x);
	scoreVector.y = (scoreVector.y >= goalColor.g) ? (scoreVector.y - goalColor.g) : (goalColor.g - scoreVector.y);
	scoreVector.z = (scoreVector.z >= goalColor.b) ? (scoreVector.z - goalColor.b) : (goalColor.b - scoreVector.z);

	score = 1000.0f - (scoreVector.x + scoreVector.y + scoreVector.z);
/*
	std::cout << "\n Score 1 -------------->" << scoreVector.x << " " << scoreVector.y << " "  << scoreVector.z << std::endl;
	std::cout << "\n Score 2 -------------->" << goalColor.r << " " << goalColor.g << " "  << goalColor.b << std::endl;
	std::cout << "\n Score 3 -------------->" << score << std::endl;
*/

	return score;
}

//The callback function is called in the curl calls in the setVariables function which sends a get request to a bulb. The callback
//function collects the bulb's response
size_t BulbHandler::callback_func(void *getInfo, size_t size, size_t count, void *stream)
{
	int hueInt, briInt, satInt, bulbIdInt, found, found2;
	std::string hue, sat, bri;
	std::string bulbId;
	std::string callbackOutput;

	//The resonse is pushed into the string vector
	bulbOutput.push_back(std::string(static_cast<const char*>(getInfo), size * count));

	//The string vector is combined into a single string
	callbackOutput = bulbOutput[0];
	if (bulbOutput.size() > 1)
	{
		for (int i = 1; i < bulbOutput.size(); i++)
		{
			callbackOutput.append(bulbOutput[i]);
			//std::cout << "\n -------------FOR LOOP------------------" << std::endl;
		}
	}
	//std::cout << "\n ----------- callbackOutput" << callbackOutput << std::endl;

	// "}}" is always the last the characters of the string, if it is found it means the whole string has been loaded
	found = callbackOutput.find("}}");

	//Finds the hue, sat, bri and hue id in the string, casts them to int and updates the static class variables
	if (found > 0)
	{

		found = callbackOutput.find("hue");
		found2 = callbackOutput.find(",", found);

		hue = callbackOutput.substr(found + 5, found2 - found - 5);

		found = callbackOutput.find("Hue Lamp");
		found2 = callbackOutput.find(",", found);

		bulbId = callbackOutput.substr(found + 9, 1);
		
		found = callbackOutput.find("sat");
		found2 = callbackOutput.find(",", found);

		sat = callbackOutput.substr(found + 5, found2 - found - 5);

		found = callbackOutput.find("bri");
		found2 = callbackOutput.find(",", found);

		bri = callbackOutput.substr(found + 5, found2 - found - 5);

		hueInt = atoi(hue.c_str());
		bulbIdInt = atoi(bulbId.c_str());
		satInt = atoi(sat.c_str());
		briInt = atoi(bri.c_str());

		if (bulbIdInt == 1)
		{
			BulbHandler::Bulb1HSV.x = hueInt;
			BulbHandler::Bulb1HSV.y = satInt;
			BulbHandler::Bulb1HSV.z = briInt;
			BulbHandler::isSetVariablesUpdated = true;
			//std::cout << "\n11111111111111111111" << std::endl;
		}
		else if (bulbIdInt == 2)
		{
			BulbHandler::Bulb2HSV.x = hueInt;
			BulbHandler::Bulb2HSV.y = satInt;
			BulbHandler::Bulb2HSV.z = briInt;
			BulbHandler::isSetVariablesUpdated = true;
			//std::cout << "\n222222222222222222" << std::endl;
		}
		else if(bulbIdInt == 3)
		{
			BulbHandler::Bulb3HSV.x = hueInt;
			BulbHandler::Bulb3HSV.y = satInt;
			BulbHandler::Bulb3HSV.z = briInt;
			BulbHandler::isSetVariablesUpdated = true;
			//std::cout << "\n33333333333333333" << std::endl;
		}
		else if (bulbIdInt == 4)
		{
			BulbHandler::Bulb4HSV.x = hueInt;
			BulbHandler::Bulb4HSV.y = hueInt;
			BulbHandler::Bulb4HSV.z = hueInt;
			BulbHandler::isSetVariablesUpdated = true;
			//std::cout << "\n44444444444444444444" << std::endl;
		}
		else
		{
			std::cout << "\nERROR: Callback function got no ID" << std::endl;
		}
	}
	return size * count;
}

//updates a bulb's global variables to match the current values the bulb is showing.
void BulbHandler::setVariables(int bulbId)
{
	 struct curl_slist *headers = NULL;
	 curl = curl_easy_init();
	 char* getInfo;
	 BulbHandler::isSetVariablesUpdated = false;

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

	 	curl_easy_perform(curl);
	 	BulbHandler::bulbOutput.clear();

	 	curl_slist_free_all(headers);

	 	curl_easy_cleanup(curl);
	 }

	 curl_global_cleanup();
}

