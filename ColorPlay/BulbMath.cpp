#include "math.h"
#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

//Calculates xyz from lab, returnes a float array with X in [0], Y in [1] and Z in [2]
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

float BulbMath::rgbTreshholdCheck(float x)
{
	float temp;
	if (x > 0.0031308)
	{
		temp = 1.055 * (pow(x, (1.0f/2.2))) - 0.055;
	}
	else
	{
		temp = 12.92 * x;
	}
	return temp;
}

//NOT finished, Seems to return right values in correspondance to eachother but in the wrong range
float BulbMath::xyz2rgb(float x, float y, float z)
{
	float R, G, B;
	float rx, ry, rz;

	rx = x/100.0f;
	ry = y/100.0f;
	rz = z/100.0f;

	/* The M matrix for the conversion
	[
    0.4124  0.3576  0.1805;
    0.2126  0.7152  0.0722;
    0.0193  0.1192  0.9505
    ];
	*/
	
	//Inverse matrix found using http://ncalculators.com/matrix/inverse-matrix.htm (not verified)

	float iMatrix[3][3] =
	{
		{0.6705, -0.3183, -0.1014},
		{-0.2005, 0.3885, 0.0065},
		{0.0115, -0.0422, 0.2189}
	};

	R = (iMatrix[0][0] * rx) + (iMatrix[0][1] * ry) + (iMatrix[0][2] * rz);
	G = (iMatrix[1][0] * rx) + (iMatrix[1][1] * ry) + (iMatrix[1][2] * rz);
	B = (iMatrix[2][0] * rx) + (iMatrix[2][1] * ry) + (iMatrix[2][2] * rz);

	R = rgbTreshholdCheck(R);
	G = rgbTreshholdCheck(G);
	B = rgbTreshholdCheck(B);

	std::cout << "\n R: " << R*255.0f << " G: " << G*255.0f << " B: " << B*255.0f << std::endl;

	return 10.0f;
}