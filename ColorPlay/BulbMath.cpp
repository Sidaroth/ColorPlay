#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

//Calculates xyz from lab, returnes a float array with X in [0], Y in [1] and Z in [2]
sf::Vector3f BulbMath::lab2xyz(float L, float a, float b)
{
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

	sf::Vector3f XYZ(X, Y, Z);

	return XYZ;

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
sf::Vector3f BulbMath::xyz2rgb(float x, float y, float z)
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

	sf::Vector3f RGB(R, G, B);

	return RGB;
}

//This function is not tested yet, math and matrix taken from rgb2lab.m documentation from Shida
sf::Vector3f BulbMath::rgb2xyz(float r, float g, float b)
{
	float X, Y, Z;

	//Threshold
	//float T = 0.008856;

	// RGB to XYZ matrix
	float M[3][3] =
	{
		{0.412453, 0.357580, 0.180423},
		{0.212671, 0.715160, 0.072169},
		{0.019334, 0.119193, 0.950227}
	};

	X = (M[0][0] * r) + (M[0][1] * g) + (M[0][2] * b);
	Y = (M[1][0] * r) + (M[1][1] * g) + (M[1][2] * b);
	Z = (M[2][0] * r) + (M[2][1] * g) + (M[2][2] * b);

	//Normalize for D65 white
	X = X / 0.950456;
	Z = Z / 1.088754;

	sf::Vector3f XYZ(X, Y, Z);	

	return XYZ;
}

//This function is not tested yet, math and matrix taken from rgb2lab.m documentation from Shida
sf::Vector3f BulbMath::xyz2lab(float x, float y, float z)
{
	float L, a, b;
	float fX, fY, fZ;

	//Threshold
	float T = 0.008856;

	float Y3 = pow(y, 3);

	if (x > T)
	{
		fX = pow(x, 1/3);
	}
	else
	{
		fX = 7.787 * x + 16.0f/116.0f;
	}

	if (y > T)
	{
		fY = Y3;
		L = (116.0f * Y3 -16.0f);
	}
	else
	{
		fY = 7.787 * y + 16.0f/116.0f;
		L = 903.3 * y;
	}

	if (z > T)
	{
		fZ = pow(z, 1/3);
	}
	else
	{
		fZ = 7.787 * z + 16.0f/116.0f;
	}

	a = 500 * (fX - fY);
	b = 200 * (fY - fZ);

	sf::Vector3f Lab(L, a, b);

	return Lab;
}

//Input = rgb in range 0 - 255, output = H in range 0 - 360, s and v in range 0 - 100
//Partly tested, worked for the ~10 values I tried according to the calculator at http://www.csgnetwork.com/csgcolorsel4.html
sf::Vector3f BulbMath::rgb2hsv(int r, int g, int b)
{
	sf::Vector3f hsv;

	float min, max, delta;

	float rr = (float)r/255;
	float gg = (float)g/255;
	float bb = (float)b/255;


	max = std::max(std::max(rr, gg), std::max(gg, bb));
	std::cout << "\nMAXXXX: " << max << std::endl;
	min = std::min(std::min(rr, gg), std::min(gg, bb));
	delta = max - min;

	hsv.z = max;

	if (max != 0)
	{
		hsv.y = delta / max;
	}
	else
	{
		//This means r, g and b are all 0, which means s = 0 and v is undefined
		hsv.x = -1;
		hsv.y = 0;
	}

	if (min == max)
	{
		hsv.x = 0;
	}
	else
	{
		if (rr == max)
		{
			std::cout << "\n1111111111" << std::endl;
			hsv.x = (gg - bb) / delta;
		}
		else if (gg == max)
		{
			std::cout << "\n2222222222" << std::endl;
			hsv.x = 2 + (bb - rr) / delta;
		}
		else
		{
			std::cout << "\n33333333333" << std::endl;
			hsv.x = 4 + (rr - gg) / delta;
		}
	}
	std::cout << "\nHER: " << hsv.x << std::endl;
	hsv.x = hsv.x * 60;
	if (hsv.x < 0)
	{
		hsv.x = hsv.x + 360;
	}

	std::cout << "\n H: " << round(hsv.x) << " s: " << round(hsv.y * 100) << " v: " << round(hsv.z * 100) << std::endl;

	return hsv;
}