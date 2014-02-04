#include "BulbHandler.hpp"

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

void BulbHandler::runCalibration(int bulbId, int low, int high, int step, int stepDelay)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	while (1)
	{
		for (int i = low; i <= high; i += step)
		{
			for (int j = 1; j <= 4; j++)
			{
				std::stringstream messageStream;
				messageStream << "{\"on\":true, \"sat\":255, \"bri\":100, \"hue\":" << i << "}";
			
				command(messageStream.str(), "PUT", j);
				std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay));
			}
		}
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "Calibration time: " << elapsed_seconds.count() << "s\n";
}