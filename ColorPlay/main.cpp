#include <chrono>
#include <thread>
#include <conio.h>
#include <string>
#include <iostream>

#include <curl/curl.h>

#define DEBUG 1
#define CURL_STATICLIB

void debugMessage(std::string message, int debugLevel);
void colorCalibration(int low = 0, int high = 65535, int step = 10, int stepDelay = 0);

void command(CURL *curl, char *body, int bulb){

	CURLcode res;
	struct curl_slist *headers = NULL;



	curl = curl_easy_init();

	if (curl) {
		printf("sending request \n");


		char* json_struct = body;
		char bulb1[] = "http://192.168.37.114/api/newdeveloper/lights/1/state";
		char bulb2[] = "http://192.168.37.114/api/newdeveloper/lights/2/state";
		char bulb3[] = "http://192.168.37.114/api/newdeveloper/lights/3/state";

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
	//colorCalibration();
	//_getch();
	CURL *curl;

	curl = curl_easy_init();

	command(curl, "{\"on\":false}", 1);

	return 0;	
}

void colorCalibration(int low, int high, int step, int stepDelay)
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	if(DEBUG > 0)
	{
		start = std::chrono::system_clock::now();
	}

	for (int i = low; i <= high; i += step)
	{
		// Do the work.
		std::cout << "Hue: " << i << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay)); // Sleep for the stepDelay. 
	}

	if (DEBUG > 0)
	{
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "Calibration time: " << elapsed_seconds.count() << "s\n";
	}
}