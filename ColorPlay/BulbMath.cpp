#include "math.h"
#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

float BulbMath::lab2xyz(float L, float a, float b)
{
	//float *pointer;
	//float xyz[3];
	//pointer = &xyz[0];

	//Treshholds
	float T1 = 0.008856;
	float T2 = 903.3;

	float fy = (L + 16) / 116;
	float fz = fy - (b / 200);
	float fx = (a / 500) + fy;

	float xr, yr, zr;

	//if ((fx^3.0f) > T1)
	if (pow(fx, 3) > T1)
	{
		xr = pow(fx, 3);
	}
	else
	{
		xr = (116 * fx - 16) / T2;
	}

	if (L > T1 * T2)
	{
		yr = pow(((L + 16) / 116), 3);
	}
	else
	{
		yr = L / T2;
	}

	//if (fz^3 > T1)
	if (pow(fz, 3) > T1)
	{
		zr = pow(fz, 3);
	}
	else
	{
		zr = (116 * fz - 16) / T2;
	}

	std::cout << "XR: " << xr << " YR: " << yr << " ZR: " << zr;

	return 10;

}