#include "BulbHandler.hpp"
#include <functional>

BulbHandler::BulbHandler()
{
	curl = curl_easy_init();
}

void BulbHandler::setBulbAdress(char* bulbAdress)
{
	this -> bulbAdress = bulbAdress;
}

void BulbHandler::addBulb(char id)
{
	bulbList.push_back(id);
}

void BulbHandler::setBrightness(int brightness, int bulbId)
{
	std::stringstream message;
	message << "{\"on\":true, \"bri\":" << brightness << "}";

	if (DEBUG >= 1)
	{
		std::cout << "Setting brightness to: " << brightness << "\n";
	}

	command(message.str(), "PUT", bulbId);
}

int BulbHandler::getBrightness(int bulbId)
{
	return 0;
}

void BulbHandler::setHue(int hue, int bulbId)
{
	std::stringstream message;
	message << "{\"on\":true, \"hue\":" << hue << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting hue to: " << hue << "\n";
	}

	command(message.str(), "PUT", bulbId);
}

int BulbHandler::getHue(int bulbId)
{
	std::stringstream message;

	commandGet();

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

	command(message.str(), "PUT", bulbId);
}

int BulbHandler::getSaturation(int bulbId)
{
	return 0;
}

void BulbHandler::command(std::string body, char* type, int bulbId)
{
	CURLcode res;
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();

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

		message << bulbAdress << bulbList[bulbId - 1] << "/state";
		
		if (type == "PUT")
		{
			message << "/state";
		}

		if (DEBUG >= 1)
		{
			std::cout << "Received body is: " << &body[0] << "\n";
			std::cout << "Bulb adress: " << &message.str()[0] << "\n";
			std::cout << "Message type is: " << type << "\n";
		}

		curl_easy_setopt(curl, CURLOPT_URL, &message.str()[0]);

		//TODO: This should be customizable I think...
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, type);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &body[0]);

		res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}


int BulbHandler::callback_func(void *getInfo, size_t size, size_t count, void *stream)
{
	char* tempString;
	char search = 'x';
	int i, j;

	std::cout << "Inni: " << &getInfo << std::endl;

	std::cout << "\n\nSTART\n";
	tempString = (char*)getInfo;

	//std::cout << tempString << std::endl << std::endl;

	for (i = 0; search != 'h'; i++)
	{
		search = tempString[i];
	}
	j = i + 4;

	for (i; search != ','; i++)
	{
		search = tempString[i];
	}
	i = i - 2;

	for (j - 1; j != i + 1; j++) //Skrive ut HUE, TODO, legge hue inn i array for � returnere i steden
	{
		std::cout << tempString[j];
	}

	std::cout << "\nEND";

	return 1337;
}

void BulbHandler::commandGet()
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

		message << bulbAdress << bulbList[0];

		/*
		if (DEBUG >= 1)
		{
			std::cout << "Received body is: " << &body[0] << "\n";
			std::cout << "Bulb adress: " << &message.str()[0] << "\n";
			std::cout << "Message type is: " << type << "\n";
		}*/

		curl_easy_setopt(curl, CURLOPT_URL, &message.str()[0]);

		//TODO: This should be customizable I think...
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &body[0]);

		std::cout << "F�r: " << &getInfo << std::endl;
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_func);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, std::bind(&BulbHandler::callback_func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getInfo);
		std::cout << "Etter: " << &getInfo << std::endl;

		res = curl_easy_perform(curl);
		std::cout << "EtterETER: " << &getInfo << std::endl;

		//std::cout << " HER :    " << res;
		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}

void BulbHandler::runCalibration(int bulbId, int low, int high, int step, int stepDelay)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int i = low; i <= high; i += step)
	{
		std::stringstream messageStream;
		messageStream << "{\"on\":true, \"sat\":255, \"bri\":100, \"hue\":" << i << "}";
		
		command(messageStream.str(), "PUT", bulbId);
		std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay));
	}

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Calibration time: " << elapsed_seconds.count() << "s\n";
}