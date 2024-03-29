/*
	Purpose: This class is responsible for handling the communication to and from the Philips Hue lightbulbs. 
			 as well as controlling the system concerned with distuinguishing the colorspaces and lightbulb cooperation. 

	Last edited: 21. Feb. 2014

	Authors: Christian Holt, Johannes Hovland, Henrik Lee Jotun
			 Gj�vik University College.


	Warning: This has become quite the "Blob" class. Should be refactored when there is time for it. 
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

BulbHandler::BulbHandler(EventQueue *eventQueue, LogModule* logger, bool* finished, bool* newGame)
{
	curl = curl_easy_init();
	currentColorSpace = ColorSpace::HSV;
	
	this -> eventQueue = eventQueue;
	gen = std::mt19937(SEED);
	increaseInterval = HSVINC;
	rgbDistribution = std::uniform_int_distribution<>(0, 255);
	this -> logger = logger;

	labAlphaA = 0.5f;
	labAlphaB = 0.5f;

	float temp = 0.0f;
	playNr = 0;
	this->currentScore = &temp;
	this->finished = finished;
	this->newGame = newGame;
	this->scoreTimer.start();
	firstGame = true;
}

void BulbHandler::init()
{
	// setHue(0, 1);
	// setHue(0, 2);
	// setHue(0, 3);
	// setHue(57000, 4);
	// setSaturation(255, 1);
	// setSaturation(255, 2);
	// setSaturation(255, 3);
	// setSaturation(255, 4);
	// setBrightness(100, 1);
	// setBrightness(100, 2);
	// setBrightness(100, 3);
	// setBrightness(100, 4);

	// setVariables(1);
	// setVariables(2);
	// setVariables(3);
	// setVariables(4);

	setHue(0, 1);
	setHue(0, 2);
	setHue(0, 3);
	setSaturation(255, 1);
	setBrightness(100, 2);
	setBrightness(100, 3);
	setBrightness(100, 1);

	loadGoalColorsFromFile();
	currentGoalColor = 0;
	setGoalColor(colors[currentGoalColor]);
}

void BulbHandler::loadGoalColorsFromFile()
{
	FILE *ifp;
	sf::Color color;
	
	ifp = fopen("colors.csv", "r");

	if (ifp == NULL) {
  		logger->LogEvent("Could not open colors.csv");
		return;
	}
	else
	{
		while(fscanf(ifp, "%d, %d, %d", &color.r, &color.g, &color.b) == 3)
		{
			color.a = 255;
			this->colors.push_back(color);
		}
	}
}

void BulbHandler::startTimer()
{
	scoreTimer.start();
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
{/*
	unsigned int r, g, b;

	r = rgbDistribution(gen);
	g = rgbDistribution(gen);
	b = rgbDistribution(gen);

	message.str(std::string());
	message << "Generated new goal color (RGB): (" << r << ", " << g << ", " << b << ")"; 
	this -> logger -> LogEvent(message.str());
	setGoalColor(r, g, b);
	message.str(std::string());
	*/

	for (int i = 0; i < colors.size(); i++)
	{
		std::cout << " " << (int)colors[i].r << " " << (int)colors[i].g << " " << (int)colors[i].b << std::endl;
	}
	
	currentGoalColor++;
	if (currentGoalColor > colors.size()-1)
	{
		currentGoalColor = 0;
	}
	setGoalColor(colors[currentGoalColor]);
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

	std::stringstream tempString;

	message.str(std::string());
	message << "ColorSpace changed to: ";

	int* colors = generateStartColors();

	switch(currentColorSpace)
	{
		case ColorSpace::RGB:
			increaseInterval = RGBINC;
			generateNewGoalColor();
			BulbHandler::Bulb1HSV = mathSuite.rgb2hsv(255,0,0);
			BulbHandler::Bulb2HSV = mathSuite.rgb2hsv(0,255,0);
			BulbHandler::Bulb3HSV = mathSuite.rgb2hsv(0,0,255);
			
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
			/*
			BulbHandler::Bulb1HSV = sf::Vector3f(44500, 255, 128);
			BulbHandler::Bulb2HSV = sf::Vector3f(56100, 255, 128);
			BulbHandler::Bulb3HSV = sf::Vector3f(21000, 255, 128);
			*/
			BulbHandler::Bulb1HSV = mathSuite.cmyk2hsv(1,0,0,0);
			BulbHandler::Bulb2HSV = mathSuite.cmyk2hsv(0,1,0,0);
			BulbHandler::Bulb3HSV = mathSuite.cmyk2hsv(0,0,1,0);
			
			message << "CMY.";
			break;

		case ColorSpace::Lab:
			increaseInterval = LABINC;
			generateNewGoalColor();
			BulbHandler::Bulb1HSV = sf::Vector3f(38800,0,100);
			BulbHandler::Bulb2HSV = mathSuite.lab2hsv(50,0,0);
			BulbHandler::Bulb3HSV = mathSuite.lab2hsv(50,0,0);
			BulbHandler::Bulb4HSV = mathSuite.lab2hsv(50,0,0);
			message << "Lab.";
			break;

		default:
			std::cerr << "Something went wrong in BulbHandler::setColorSpace(). Unkown ColorSpace";
			break;
	}

	tempString << "{\"on\":true,\"hue\": " << BulbHandler::Bulb1HSV.x << ", \"sat\": " << BulbHandler::Bulb1HSV.y << ", \"bri\": " <<  BulbHandler::Bulb1HSV.z << "}";
	command(tempString.str(), 1);
	tempString.str(std::string());
	tempString << "{\"on\":true,\"hue\": " << BulbHandler::Bulb2HSV.x << ", \"sat\": " << BulbHandler::Bulb2HSV.y << ", \"bri\": " <<  BulbHandler::Bulb2HSV.z << "}";
	command(tempString.str(), 2);
	tempString.str(std::string());
	tempString << "{\"on\":true,\"hue\": " << BulbHandler::Bulb3HSV.x << ", \"sat\": " << BulbHandler::Bulb3HSV.y << ", \"bri\": " <<  BulbHandler::Bulb3HSV.z << "}";
	command(tempString.str(), 3);
	tempString.str(std::string());
	tempString << "{\"on\":true,\"hue\": " << "21000" << ", \"sat\": " << "0" << ", \"bri\": " <<  "128" << "}";
	command(tempString.str(), 4);
				

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

		BulbHandler::Bulb4HSV.x = BulbHandler::Bulb1HSV.x;

		// Update the other lightbulbs

		helper = message.str();
		std::async(std::launch::async, &BulbHandler::command, this, helper, 2);
		command(helper, 3);

	}
	else if(bulbId == 2)		// S
	{
		if (inc < 0)
			val = BulbHandler::Bulb2HSV.y - SATINC;
		else
			val = BulbHandler::Bulb2HSV.y + SATINC;

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

		BulbHandler::Bulb4HSV.y = BulbHandler::Bulb2HSV.y;

		helper = message.str();
		std::async(std::launch::async, &BulbHandler::command, this, helper, 1);
		command(helper, 3);	

	}
	else if(bulbId == 3)   // V
	{
		if (inc > 0)
			val = BulbHandler::Bulb3HSV.z + BRIINC;
		else
			val = BulbHandler::Bulb3HSV.z - BRIINC;

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

		BulbHandler::Bulb4HSV.z = BulbHandler::Bulb3HSV.z;

		helper = message.str();
		std::async(std::launch::async, &BulbHandler::command, this, helper, 1);
		command(helper, 2);	
	}

	// Update the data on this and the target light bulbs. 
	std::async(std::launch::async, &BulbHandler::command, this, helper, 4);
	command(helper, bulbId);
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

	// Update the data on this light bulb asynchronously. 
	std::async(std::launch::async, &BulbHandler::command, this, message.str(), bulbId);

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
	if(bulbId == 1)		// C
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
		values.x = values.x + ((float)inc/100);

		BulbHandler::Bulb1HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
		message << "\"hue\": " << BulbHandler::Bulb1HSV.x << ", \"sat\": " << BulbHandler::Bulb1HSV.y << ", \"bri\": " <<  BulbHandler::Bulb1HSV.z << "}";
	}
	else if(bulbId == 2) // M
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
		values.y = values.y + ((float)inc/100);
		BulbHandler::Bulb2HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
		message << "\"hue\": " << BulbHandler::Bulb2HSV.x << ", \"sat\": " << BulbHandler::Bulb2HSV.y << ", \"bri\": " <<  BulbHandler::Bulb2HSV.z << "}";
	}
	else if(bulbId == 3) // Y
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);
		values.z = values.z + ((float)inc/100);
		BulbHandler::Bulb3HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
		message << "\"hue\": " << BulbHandler::Bulb3HSV.x << ", \"sat\": " << BulbHandler::Bulb3HSV.y << ", \"bri\": " <<  BulbHandler::Bulb3HSV.z << "}";
	}

	command(message.str(), bulbId);

	sf::Vector3f CMYKtemp1 = mathSuite.hsv2cmyk(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
	sf::Vector3f CMYKtemp2 = mathSuite.hsv2cmyk(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
	sf::Vector3f CMYKtemp3 = mathSuite.hsv2cmyk(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);

	values.x = CMYKtemp1.x + CMYKtemp2.x + CMYKtemp3.x;
	values.y = CMYKtemp1.y + CMYKtemp2.y + CMYKtemp3.y;
	values.z = CMYKtemp1.z + CMYKtemp2.z + CMYKtemp3.z;

	values = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);

	message.str(std::string());
	message << "{\"on\":true, \"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";

	// Update target light bulb in the background, asynchronously. 
	std::async(std::launch::async, &BulbHandler::command, this, message.str(), 4);
	
	/*
	if(bulbId == 1)
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.x = values.x + ((float)inc/100);

		BulbHandler::Bulb4HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
	}
	else if(bulbId == 2)
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.y = values.y + ((float)inc/100);

		BulbHandler::Bulb4HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
	}
	else if(bulbId == 3)
	{
		values = mathSuite.hsv2cmyk(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.z = values.z + ((float)inc/100);

		BulbHandler::Bulb4HSV = mathSuite.cmyk2hsv(values.x, values.y, values.z, 0);
	}
	else
	{
		std::cout << "\nError in CMY Color Adjustment, no valid bulbID" << std::endl;
	}

	message << "\"hue\": " << BulbHandler::Bulb4HSV.x << ", \"sat\": " << BulbHandler::Bulb4HSV.y << ", \"bri\": " <<  BulbHandler::Bulb4HSV.z << "}";
	command(message.str(), 4);
	*/
}



/// Depending on the event received, change the corresponding bulb (L, a, or b value)
void BulbHandler::LabColorAdjustment(unsigned short bulbId, short inc)
{
	
	if(bulbId == 1) // L
	{
		//inc is divided by 3.6 because the L values is in 0 - 100 while a and b are in -180 to 180
		values = mathSuite.hsv2lab(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
		values.x = values.x  + inc / 3.6;
		BulbHandler::Bulb1HSV = mathSuite.lab2hsv(values.x, values.y, values.z);
	}
	else if(bulbId == 2) // a
	{
		values = mathSuite.hsv2lab(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
		std::cout << "\n-------------->" << values.x << " " << values.y << " " << values.z << " " << inc << std::endl;
		values.y = values.y + inc;
		std::cout << "\n-------------->" << values.x << " " << values.y << " " << values.z << " " << inc << std::endl;

		BulbHandler::Bulb2HSV = mathSuite.lab2hsv(values.x, values.y, values.z);
	}
	else if(bulbId == 3) // b
	{
		values = mathSuite.hsv2lab(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);
		values.z = values.z  + inc;
		BulbHandler::Bulb3HSV = mathSuite.lab2hsv(values.x, values.y, values.z);
	}

	values = mathSuite.lab2hsv(values.x, values.y, values.z);
	message << "\"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";

	std::cout << (mathSuite.lab2hsv(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z)).y;

	command(message.str(), bulbId);

	sf::Vector3f Labtemp1 = mathSuite.hsv2lab(BulbHandler::Bulb1HSV.x, BulbHandler::Bulb1HSV.y, BulbHandler::Bulb1HSV.z);
	sf::Vector3f Labtemp2 = mathSuite.hsv2lab(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z);
	sf::Vector3f Labtemp3 = mathSuite.hsv2lab(BulbHandler::Bulb3HSV.x, BulbHandler::Bulb3HSV.y, BulbHandler::Bulb3HSV.z);

	values.x = Labtemp1.x + Labtemp2.x + Labtemp3.x;
	values.y = Labtemp1.y + Labtemp2.y + Labtemp3.y;
	values.z = Labtemp1.z + Labtemp2.z + Labtemp3.z;

	values = mathSuite.lab2hsv(values.x, values.y, values.z);

	message.str(std::string());
	message << "{\"on\":true, \"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";

	// Update target light bulb in the background, asynchronously. 
	std::async(std::launch::async, &BulbHandler::command, this, message.str(), 4);

	std::cout << (mathSuite.lab2hsv(BulbHandler::Bulb2HSV.x, BulbHandler::Bulb2HSV.y, BulbHandler::Bulb2HSV.z)).y;



	//std::string helper = message.str();
	//std::async(std::launch::async, &BulbHandler::command, this, message.str(), 4);

/*
	for(int i = 1; i < 4; ++i)
	{
		command(helper, i);
	}
*/	

	/*
	if (bulbId == 1)
	{	
		if (inc > 0)
		{
			BulbHandler::Bulb1HSV.z = BulbHandler::Bulb1HSV.z + 10;
			BulbHandler::Bulb4HSV.x = BulbHandler::Bulb4HSV.x + 10;
		}
		else
		{ 
			BulbHandler::Bulb1HSV.z = BulbHandler::Bulb1HSV.z - 10;
			BulbHandler::Bulb4HSV.x = BulbHandler::Bulb4HSV.x - 10;
		}

		if (BulbHandler::Bulb1HSV.z < MINBRI)
			BulbHandler::Bulb1HSV.z = MINBRI;
		else if(BulbHandler::Bulb1HSV.z > MAXBRI)
			BulbHandler::Bulb1HSV.z = MAXBRI;

		values = BulbHandler::Bulb1HSV;


	}
	else if(bulbId == 2)
	{	//The increment is divided by 360 for get 0.1 which is 10 steps in the alpha value
		//labAlphaA = labAlphaA + (((float)inc / 100) / 2);
		values = mathSuite.hsv2lab(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.y = values.y + inc;
		BulbHandler::Bulb4HSV = mathSuite.lab2hsv(values.x, values.y, values.z);

		if (inc > 0)
			labAlphaA = labAlphaA + 0.1f;
		else
			labAlphaA = labAlphaA - 0.1f;

		if (labAlphaA < 0)
			labAlphaA = 0.0f;
		else if (labAlphaA > 1)
			labAlphaA = 1.0f;
		values = mathSuite.hsv2rgb(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.x = 255*labAlphaA;
		values.y = 255*(1 - labAlphaA);		
		values.z = 0;
		values = mathSuite.rgb2hsv(values.x, values.y, values.z);
	}
	else if(bulbId == 3)
	{
		values = mathSuite.hsv2lab(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.z = values.z + inc;
		BulbHandler::Bulb4HSV = mathSuite.lab2hsv(values.x, values.y, values.z);

         //labAlphaB = labAlphaB + (((float)inc/100) / 2);
		if (inc > 0)
			labAlphaB = labAlphaB + 0.1f;
		else
			labAlphaB = labAlphaB - 0.1f;

		if (labAlphaB < 0)
			labAlphaB = 0.0f;
		else if (labAlphaB > 1)
			labAlphaB = 1.0f;
		values = mathSuite.hsv2rgb(BulbHandler::Bulb4HSV.x, BulbHandler::Bulb4HSV.y, BulbHandler::Bulb4HSV.z);
		values.x = 255*labAlphaB;
		values.y = 255*labAlphaB;
		values.z = 255*(1 - labAlphaB);
		values = mathSuite.rgb2hsv(values.x, values.y, values.z);
	}


	message << "\"hue\": " << values.x << ", \"sat\": " << values.y << ", \"bri\": " <<  values.z << "}";
	command(message.str(), bulbId);

	message.str(std::string());
	message << "{\"on\":true,\"hue\": " << BulbHandler::Bulb4HSV.x << ", \"sat\": " << BulbHandler::Bulb4HSV.y << ", \"bri\": " << BulbHandler::Bulb4HSV.z << "}";
	command(message.str(), 4);
	*/
}

/// Depending on the event received, change the corresponding bulb (X, Y, or Z value)
/// USED?
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

	message  << "\"hue\": " << BulbHandler::Bulb3HSV.x << ", \"sat\": " << BulbHandler::Bulb3HSV.y << ", \"bri\": " << BulbHandler::Bulb3HSV.z << "}";
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

	 	if (firstGame == true)
	 	{
	 		firstGame = false;
	 		actionTimer.start();
	 	}
	 	if(currentAction != lastAction || currentAction.getBulbID() != lastAction.getBulbID())
	 	{
	 		lastAction = currentAction;
	 		int timeElapsed = actionTimer.secondsElapsed();
	 		actionTimer.start();
	 		writeAction(lastAction, timeElapsed);
	 	}

		switch(currentAction.action)
		{
			case ActionEvent::Action::Up:
				bulb = currentAction.getBulbID();
				updateBulb(bulb, increaseInterval);
				break;

			case ActionEvent::Action::Down:
				bulb = currentAction.getBulbID();
				updateBulb(bulb, increaseInterval * -1);
				break;

			case ActionEvent::Action::Finish:
				if(!(*this->finished))
				{
					std::cout << std::endl << std::this_thread::get_id() << std::endl;
					int temp = this->scoreTimer.secondsElapsed();
					this->scoreTimer.stop();
					calculateScore(temp);
					*this->finished = true;
				}
				else if (*this->newGame)
				{
					startNewGame();
					*this -> newGame = false;
				}
				break;

			case ActionEvent::Action::None:
				break;

			default: 
				break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

bool BulbHandler::doesFileExist(std::string name)
{
	std::ifstream f(name.c_str());

	if (f.good())
	{
		f.close();
		return true;
	}
	else
	{
		f.close();
		return false;
	}
}

void BulbHandler::writeAction(ActionEvent action, int timeUsed)
{
	std::chrono::time_point<std::chrono::system_clock> now;
	std::stringstream date;
	char buffer[BUFFERSIZE];
	struct tm * timeinfo;
	std::time_t logTime;

	now = std::chrono::system_clock::now();
	logTime = std::chrono::system_clock::to_time_t(now);

	time(&logTime);
	timeinfo = localtime(&logTime);
	strftime(buffer, BUFFERSIZE, "%F", timeinfo);
	date << "Actions - " << buffer << ".csv";


	if (!doesFileExist(date.str()))
	{
		std::ofstream stfInit(date.str());
		stfInit << "PlayNr" << "\tTime Stamp," << "\tTime Used" << "\tBulb ID," << "\tAction," << "\tColor Space" << std::endl;
		stfInit.close();
	}

	std::ofstream actionFile(date.str(), std::ios::app);
	
	strftime(buffer, BUFFERSIZE, "%T", timeinfo);

	actionFile << playNr << ",\t" << buffer << ",\t" << timeUsed << ",\t" << action.getBulbID() << ",\t";

	switch (action.action)
	{
		case ActionEvent::Action::Up:
			actionFile << "Up,\t";
			break;
		case ActionEvent::Action::Down:
			actionFile << "Down,\t";
			break;
		case ActionEvent::Action::Finish:
			actionFile << "Finish,\t";
			break;
		case ActionEvent::Action::None:
			actionFile << "None,\t";
			break;
		default:
			std::cout << "\nError in Write Action, invalid action" << std::endl;
			break;
	}

	switch (this->currentColorSpace)
	{
		case ColorSpace::RGB:
			actionFile << "RGB" << std::endl;
			break;
		case ColorSpace::HSV:
			actionFile << "HSV" << std::endl;
			break;
		case ColorSpace::Lab:
			actionFile << "Lab" << std::endl;
			break;
		case ColorSpace::CMY:
			actionFile << "CMY" << std::endl;
			break;
		default:
			std::cout << "\nError in write action, invalid colorspace" << std::endl;
			break;
	}
	actionFile.close();

}

//this needs more testing to check date change logging
//writes score in a separate file "Score - <date>" and score data (time, goal color and achived color) in a file. "Score Data/Time <date>"
//The score data file is in the format <time>, <achived color X>, <Y>, <Z>, <goal color X>, <Y>, <Z>
void BulbHandler::writeScoreAndTime(float score, int timeUsed)
{
	std::chrono::time_point<std::chrono::system_clock> now;
	std::stringstream date;
	char buffer[BUFFERSIZE];
	struct tm * timeinfo;
	std::time_t logTime;
	sf::Color goalColor = getGoalColor();
	sf::Vector3f tempGoalColor;

	now = std::chrono::system_clock::now();
	logTime = std::chrono::system_clock::to_time_t(now);

	time(&logTime);
	timeinfo = localtime(&logTime);
	strftime(buffer, BUFFERSIZE, "%F",timeinfo);

	date << "Score - " << buffer << ".txt";

	if (!doesFileExist(date.str()))
	{
		scoreVector.clear();
	}

	scoreVector.push_back(score);
	std::sort(scoreVector.begin(), scoreVector.end());
	std::reverse(scoreVector.begin(), scoreVector.end());
	
	if (scoreVector.size() > 50)
	{
		scoreVector.pop_back();
	}

	std::ofstream scoreFile(date.str());

	for (int i = 0; i < scoreVector.size(); i++)
	{
		scoreFile << i+1 << ". \t" << scoreVector[i] << std::endl;
	}
	scoreFile.close();

	//reuse date stringstream to get score time filename. scoreTime records the time players used
	date.str("");
	date << "Score Data and Time - " << buffer;

	if (this -> currentColorSpace == ColorSpace::RGB)	
	{
		date << " - RGB.csv";
	}
	else if (this -> currentColorSpace == ColorSpace::HSV)
	{
		date << " - HSV.csv";
		tempGoalColor = mathSuite.rgb2hsv((float)goalColor.r, (float)goalColor.g, (float)goalColor.b); 
	}	
	else if (this -> currentColorSpace == ColorSpace::XYZ)
	{
		date << " - XYZ.csv";
		tempGoalColor = mathSuite.rgb2xyz((float)goalColor.r, (float)goalColor.g, (float)goalColor.b); 
	}
	else if (this -> currentColorSpace == ColorSpace::Lab)
	{
		date << " - LAB.csv";
		tempGoalColor = mathSuite.rgb2xyz((float)goalColor.r, (float)goalColor.g, (float)goalColor.b); 
		tempGoalColor = mathSuite.xyz2lab(tempGoalColor.x, tempGoalColor.y, tempGoalColor.z); 
	}
	else if (this -> currentColorSpace == ColorSpace::CMY)
	{
		date << " - CMY.csv";
		tempGoalColor = mathSuite.rgb2cmyk((float)goalColor.r, (float)goalColor.g, (float)goalColor.b);
		//Todo, fix cmyk write to file
	}

	if (!doesFileExist(date.str()))
	{
		std::ofstream stfInit(date.str());
		stfInit << "PlayNr" << "\tTime," << "\tTC1," << "\tTC2," << "\tTC3," << "\tPC1," << "\tPC2," << "\tPC3" << std::endl;
		stfInit.close();
	}

	std::ofstream scoreTimeFile(date.str(), std::ios::app);
	//scoreTimeFile << timeUsed << ",\t" << BulbHandler::Bulb4HSV.x << ",\t" << BulbHandler::Bulb4HSV.y << ",\t" << BulbHandler::Bulb4HSV.z << ",\t" << tempGoalColor.x << ",\t" << tempGoalColor.y << ",\t" << tempGoalColor.z << std::endl;
	scoreTimeFile << playNr << ",\t" << timeUsed << ",\t" << tempGoalColor.x << ",\t" << tempGoalColor.y << ",\t" << tempGoalColor.z << ",\t" << BulbHandler::Bulb4HSV.x << ",\t" << BulbHandler::Bulb4HSV.y << ",\t" << BulbHandler::Bulb4HSV.z << std::endl;

	scoreTimeFile.close();
}

//Calculates the score based on how far from the goal color the current color in bulb 4 is. All scoring is done in RGB colorSpace for simplicity
//calculateScore should be called when the player signals that they are done mixing a color
float BulbHandler::calculateScore(int timeUsed)
{
	float score;
	sf::Vector3f scoreVector;
	sf::Color goalColor = getGoalColor();

	scoreVector = mathSuite.rgb2hsv((float)goalColor.r, (float)goalColor.g, (float)goalColor.b);

	// Euclidean_distance 3d calculations
	float tempr = pow((scoreVector.x/MAXHUE*MAXSAT) - (BulbHandler::Bulb4HSV.x/MAXHUE*MAXSAT), 2);
	float tempg = pow(scoreVector.y - BulbHandler::Bulb4HSV.y, 2);
	float tempb = pow(scoreVector.z - BulbHandler::Bulb4HSV.z, 2);
	score = 10000 - ((sqrt(tempr + tempg + tempb))/(255*sqrt(3))*10000);

//	std::cout << "\n TID BRUKT ----------->" << timeUsed << std::endl;
//	std::cout << "\n SCORE------------->" << score << std::endl;

	writeScoreAndTime(score, timeUsed);
	tempScore = score;

	currentScore = &tempScore;
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
			BulbHandler::Bulb4HSV.y = satInt;
			BulbHandler::Bulb4HSV.z = briInt;
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

void BulbHandler::startNewGame()
{
	setColorSpace(this->currentColorSpace);
	playNr++;
	this->scoreTimer.start();
	*this->finished = false;
}
