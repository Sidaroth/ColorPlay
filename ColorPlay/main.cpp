#include <chrono>
#include <thread>
#include <conio.h>
#include <string>
#include <iostream>
#include <sstream>

#include <curl/curl.h>

#define DEBUG 1
#define CURL_STATICLIB

void debugMessage(std::string message, int debugLevel);
void colorCalibration(int bulb = 1, int low = 0, int high = 65535, int step = 10, int stepDelay = 0);
int *getTestSettings();//Asks the user for the test parameters

void command(CURL *curl, char *body, int bulb){

	CURLcode res;
	struct curl_slist *headers = NULL;



	curl = curl_easy_init();

	if (curl) {
		printf("Sending request \n");


		char* json_struct = body;
		char bulb1[] = "http://192.168.37.114/api/newdeveloper/lights/1/state";
		char bulb2[] = "http://192.168.37.114/api/newdeveloper/lights/2/state";
		char bulb3[] = "http://192.168.37.114/api/newdeveloper/lights/3/state";
		char bulb4[] = "http://192.168.37.114/api/newdeveloper/lights/4/state";

		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		if (bulb == 1)
			curl_easy_setopt(curl, CURLOPT_URL, bulb1);
		else if (bulb == 2)
			curl_easy_setopt(curl, CURLOPT_URL, bulb2);
		else if (bulb == 3)
			curl_easy_setopt(curl, CURLOPT_URL, bulb3);
		else if (bulb == 4)
			curl_easy_setopt(curl, CURLOPT_URL, bulb4);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_struct);

		res = curl_easy_perform(curl);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

}

int main(int argc, char* argv[])
{
	int *settings = getTestSettings();

	colorCalibration(settings[0], settings[1], settings[2], settings[3], settings[4]);

	std::cout << "\nFinished, press any key to exit.";

	_getch();
	return 0;	
}

void colorCalibration(int bulb, int low, int high, int step, int stepDelay)
{
	CURL *curl;

	curl = curl_easy_init();

	std::chrono::time_point<std::chrono::system_clock> start, end;
	if(DEBUG > 0)
	{
		start = std::chrono::system_clock::now();
	}

	for (int i = low; i <= high; i += step)
	{
		// Do the work.
		std::cout << "\nHue: " << i << "\n";
		std::stringstream messageStream;
		messageStream << "{\"on\":true, \"sat\":255, \"bri\":100, \"hue\":" << i << "}";

		std::cout << "Message: " << messageStream.str() << "\n\n";

		std::string message = messageStream.str();

		command(curl, &message[0], bulb);
		std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay)); // Sleep for the stepDelay. 
	}

	if (DEBUG > 0)
	{
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "Calibration time: " << elapsed_seconds.count() << "s\n";
	}
}

//Asks the user for the test parameters
int *getTestSettings()
{
	int *settings = new int[5];

	while (settings[0] < 1 || 4 < settings[0])
	{
		std::cout << "\nPlease enter the number of the bulb (1-4): ";
		std::cin >> settings[0];
	}
	while (settings[1] < 0 || 65535 < settings[1])
	{
		std::cout << "\nPlease enter the low hue threshold (0-65535): ";
		std::cin >> settings[1];
	}
	while (settings[2] < 0 || 65535 < settings[2])
	{
		std::cout << "\nPlease enter the high hue threshold (0-65535): ";
		std::cin >> settings[2];
	}
	while (settings[3] < 1 || 10000 < settings[3])
	{
		std::cout << "\nPlease enter the step increment (1-10000): ";
		std::cin >> settings[3];
	}
	while (settings[4] < 0 || 65535 < settings[4])
	{
		std::cout << "\nPlease enter the step delay in ms (0-1800000): "; //0ms to 30min
		std::cin >> settings[4];
	}

	return settings;
}