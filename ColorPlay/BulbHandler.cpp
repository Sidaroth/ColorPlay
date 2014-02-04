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

void BulbHandler::setBrightness(int brightness)
{

}

int BulbHandler::getBrightness()
{
	return 0;
}

void BulbHandler::setHue(int hue)
{
	std::stringstream message;
	message << "{\"on\":true, \"hue\":" << hue << "}";
	
	if (DEBUG >= 1)
	{
		std::cout << "Setting hue to: " << hue << "\n";
	}

	command(message.str(), "PUT");
}

int BulbHandler::getHue()
{
	return 0;
}


void BulbHandler::setSaturation(int saturation)
{
	
}

int BulbHandler::getSaturation()
{
	return 0;
}

//char* BulbHandler::buildBody(std::string message)
//{
//	std::copy(message.begin(), message.end(), messageString);
//
//	return &messageString[0];
//}

void BulbHandler::command(std::string body, char* type)
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

		message << bulbAdress << bulbList[0] << "/state";

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