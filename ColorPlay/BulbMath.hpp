// Header file for BulbMath used for color conversion 

#include <iostream>

class BulbMath
{
public: 
	BulbMath();
	float *lab2xyz(float L, float a, float b);
	float xyz2rgb(float x, float y, float z);

private:
	float rgbTreshholdCheck(float x);

};