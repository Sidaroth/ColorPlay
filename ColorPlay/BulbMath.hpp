// Header file for BulbMath used for color conversion 

#include <iostream>
#include <SFML/System/Vector3.hpp>
#include "math.h"
#include <algorithm>

class BulbMath
{
public: 
	BulbMath();
	sf::Vector3f lab2xyz(float L, float a, float b);
	sf::Vector3f xyz2rgb(float x, float y, float z);
	sf::Vector3f rgb2xyz(float r, float g, float b);
	sf::Vector3f xyz2lab(float x, float y, float z);
	sf::Vector3f rgb2hsv(int r, int g, int b);
	sf::Vector3f hsv2rgb(float H, float s, float v);


private:
	float rgbTreshholdCheck(float x);

};