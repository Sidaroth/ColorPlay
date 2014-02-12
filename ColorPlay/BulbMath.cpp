#include "math.h"
#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

float *BulbMath::lab2xyz(float L, float a, float b)
{
	float *pointer;
	float xyz[3];
	pointer = &xyz[0];

	//Treshholds
	float T1 = 0.008856;
	float T2 = 903.3;

	float fX, fY, fZ;
	//float xr, yr, zr;
	float X, Y, Z;
	/*
	Lindbloom version
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
	*/

	//Math from shida
	//Calculate Y
	fY = pow(((L + 16) / 116), 3);

	if (fY <= T1)
	{
		fY = L / 903.3;
	}

	Y = fY;

	//Slightly change fY for further calculations
	/*if (fY > T1)
	{
		fY = pow(fY, (1/3));
	}
	else
	{
		fY = (7.787 * fY + 16/116);
	}*/

	//Calculate X
	fX = (a / 500) + fY;

	if (fX > T2)
	{
		X = pow(fX, 3);
	}
	else
	{
		X = fX - ((16.0f/116.0f) / 7.7877);
	}

	//Calculate Z
	fZ = fY - (b / 200.0f);

	if (fZ > T2)
	{
		Z = pow(fZ, 3);
	}
	else
	{
		Z = fZ - ((16.0f/116.0f) / 7.7877);
	}

	//Normalize for D65 white point
	X = X * 0.950456;
	Z = Z * 1.088754;

	std::cout << "\n X: " << X << " Y: " << Y << " Z: " << Z << std::endl;

	xyz[0] = X;
	xyz[1] = Y;
	xyz[2] = Z;

	return pointer;

}