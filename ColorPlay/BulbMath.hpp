// Header file for BulbMath used for color conversion 

#pragma once

#include <iostream>
#include <SFML/System/Vector3.hpp>
#include <algorithm>
#include "EventQueue.hpp"
#include "ActionEvent.hpp"

#include "math.h"

class BulbMath
{
public: 
	BulbMath();

	// Lab -> 
	sf::Vector3f lab2hsv(float L, float a, float b);
	sf::Vector3f lab2xyz(float L, float a, float b);

	// XYZ -> 
	sf::Vector3f xyz2hsv(float x, float y, float z);
	sf::Vector3f xyz2rgb(float x, float y, float z);
	sf::Vector3f xyz2lab(float x, float y, float z);
	
	// RGB -> 
	sf::Vector3f rgb2xyz(float r, float g, float b);
	float * rgb2cmyk(float r, float g, float b);
	sf::Vector3f rgb2hsv(float r, float g, float b);

	// HSV -> 
	sf::Vector3f hsv2rgb(float H, float s, float v);
	sf::Vector3f hsv2lab(float h, float s, float v);
	float * hsv2cmyk(float h, float s, float v);
	sf::Vector3f hsv2xyz(float h, float s, float v);

	// CMYK -> 
	sf::Vector3f cmyk2rgb(float c, float m, float y, float k);
	sf::Vector3f cmyk2hsv(float c, float m, float y, float k);


private:
	EventQueue *eventQueue;
	sf::Vector3f rgbThresholdCheck(sf::Vector3f rgb);
	sf::Vector3f xyzThresholdCheck(sf::Vector3f xyz);
	sf::Vector3f labThresholdCheck(sf::Vector3f lab);
	sf::Vector3f hsvThresholdCheck(sf::Vector3f hsv);
	sf::Vector3f cmyThresholdCheck(sf::Vector3f cmy);
};