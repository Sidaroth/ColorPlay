#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

//Returns XYZ in range 0 - 100.
//Works according to calculator at www.google.no/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0CC0QFjAA&url=http%3A%2F%2Fwww.brucelindbloom.com%2F&ei=qFr7UpDgGKP9ywO-qILgBw&usg=AFQjCNF2xEuBjOP60Afxlj2BMU8lhYlaMA&bvm=bv.61190604,d.bGQ
//remember to set reference white to D65 when using calculator
sf::Vector3f BulbMath::lab2xyz(float L, float a, float b)
{
	
	//Tristimulus values of reference white D(65)
	float Xn = 95.047/100;
	float Yn = 100.0/100;
	float Zn = 108.883/100;
	float fx, fy, fz, xr, yr, zr, X, Y, Z;

	//Tresholds
	float T1 = 0.008856;
	float T2 = 903.3;

	fy = (L + 16) / 116;
	fz = fy - b / 200;
	fx = a / 500 + fy;

	if (pow(fx, 3) > T1)
	{
		xr = pow(fx, 3);
	}
	else
	{
		xr = (116 * fx - 16) / T2;
	}

	if (L > (T1 * T2))
	{
		yr = pow(((L + 16) / 116), 3);
	}
	else
	{
		yr = L / T2;
	}

	if (pow (fz, 3) > T1)
	{
		zr = pow(fz, 3);
	}
	else
	{
		zr = (116 * fz - 16) / T2;
	}
	X = xr * Xn;
	Y = yr * Yn;
	Z = zr * Zn;


	sf::Vector3f XYZ(X*100, Y*100, Z*100);
	//std::cout << "X: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

	return XYZ;

}


//Working. Tested with bruce lindbloom calculator (remember to set d65 white point)
//input XYZ in rage 0 - 100
sf::Vector3f BulbMath::xyz2lab(float X, float Y, float Z)
{
	float xr, yr, zr, fx, fy, fz, L, a, b;

	//D65 Tristimulus values
	float Xn = 95.047;
	float Yn = 100.0;
	float Zn = 108.883;

	//Treshold
	float T1 = 0.008856;
	float T2 = 903.3;

	xr = X / Xn;
	yr = Y / Yn;
	zr = Z / Zn;

	if (xr > T1)
	{
		fx = cbrt(xr);
	}
	else
	{
		fx = (T2 * xr + 16) / 116;
	}

	if (yr > T1)
	{
		fy = cbrt(yr);
	}
	else
	{
		fy = (T2 * yr + 16) / 116;
	}

	if (zr > T1)
	{
		fz = cbrt(zr);
	}
	else
	{
		fz = (T2 * zr + 16) / 116;
	}

	L = 116 * fy - 16;
	a = 500 * (fx - fy);
	b = 200 * (fy - fz);

	sf::Vector3f LAB(L, a, b);
	std::cout << "\nL: " << L << " a: " << a << " b: " << b << std::endl;

	return LAB;

}

float BulbMath::rgbTreshholdCheck(float x)
{
	if (x < 0)
	{
		return 0.0f;
	}
	else if (x > 1)
	{
		return 1.0f;
	}
	else
	{
		return x;
	}
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
/*
	float iMatrix[3][3] =
	{
		{2.7454669, -0.4836786, -0.4350259},
		{-0.7710229, 1.7065571, 0.0446900},
		{0.0400013, -0.0885376, 1.0132541}
	};
*/
/*
	//Bruce RGB t
	float iMatrix[3][3] =
	{
		{2.7454669, -1.1358136, -0.4350259},
		{-0.9692660, 1.8760108, 0.0415560},
		{0.0112723, -0.1139754, 1.0132541}
	};
	*/
	/*
	//SRGB
	float iMatrix[3][3] =
	{
		{0.4124564, 0.3575761, 0.1804375},
		{0.2126729, 0.7151522, 0.0721750},
		{0.0193339, 0.1191920, 0.9503041}
	};
*/
	//From color
	float iMatrix[3][3] =
	{
		{3.240479, -1.537150, -0.498535},
		{-0.969256, 1.875992, 0.041556},
		{0.055648, -0.204043, 1.057311}
	};

	std::cout << "\nrx " << rx << " ry: " << ry << " rz: " << rz << std::endl; 

	R = (iMatrix[0][0] * rx) + (iMatrix[0][1] * ry) + (iMatrix[0][2] * rz);
	G = (iMatrix[1][0] * rx) + (iMatrix[1][1] * ry) + (iMatrix[1][2] * rz);
	B = (iMatrix[2][0] * rx) + (iMatrix[2][1] * ry) + (iMatrix[2][2] * rz);

	std::cout << "\nfR " << R << " fG: " << G << " fB: " << B << std::endl; 

	R = rgbTreshholdCheck(R);
	G = rgbTreshholdCheck(G);
	B = rgbTreshholdCheck(B);

	std::cout << "\nR " << R << " G: " << G << " B: " << B << std::endl; 


	sf::Vector3f RGB(R*255, G*255, B*255);

	std::cout << "\nVR " << RGB.x << " VG: " << RGB.y << " VB: " << RGB.z << std::endl; 

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
			hsv.x = (gg - bb) / delta;
		}
		else if (gg == max)
		{
			hsv.x = 2 + (bb - rr) / delta;
		}
		else
		{
			hsv.x = 4 + (rr - gg) / delta;
		}
	}
	hsv.x = hsv.x * 60;
	if (hsv.x < 0)
	{
		hsv.x = hsv.x + 360;
	}

	//std::cout << "\n H: " << round(hsv.x) << " s: " << round(hsv.y * 100) << " v: " << round(hsv.z * 100) << std::endl;

	return hsv;
}

//H in value 0 - 360, s and v in range 0 - 100; return rgb in range 0 - 255.
//Tested and works according to calculator @ http://www.csgnetwork.com/csgcolorsel4.html
sf::Vector3f BulbMath::hsv2rgb(float H, float s, float v)
{
	sf::Vector3f RGB;
	float f, p, q, t, hh, ss, vv;
	int i;

	ss = s / 100;
	vv = v / 100;

	if (s == 0)		//This means the color is grey
	{
		RGB.x = RGB.y = RGB.z = vv;
		RGB.x = round(RGB.x * 255);
		RGB.y = round(RGB.y * 255);
		RGB.z = round(RGB.z * 255);

		//std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;
		return RGB;
	}

	hh = H / 60;
	i = floor(hh);
	f = hh - i;
	p = vv * (1 - ss);
	q = vv * (1 - ss * f);
	t = vv * (1 - ss * (1 - f));

	switch(i) {
		case 0:
				RGB.x = vv;
				RGB.y = t;
				RGB.z = p;
				break;
		case 1:
				RGB.x = q;
				RGB.y = vv;
				RGB.z = p;
				break;
		case 2:
				RGB.x = p;
				RGB.y = vv;
				RGB.z = t;
				break;
		case 3:
				RGB.x = p;
				RGB.y = q;
				RGB.z = vv;
				break;
		case 4:
				RGB.x = t;
				RGB.y = p;
				RGB.z = vv;
				break;
		default:
				RGB.x = vv;
				RGB.y = p;
				RGB.z = q;
				break;
	}

	RGB.x = round(RGB.x * 255);
	RGB.y = round(RGB.y * 255);
	RGB.z = round(RGB.z * 255);
	//std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;
	return RGB;
}

sf::Vector3f BulbMath::cmyk2rgb(float c, float m, float y, float k)
{
	sf::Vector3f RGB;

	return RGB;
}

//FOrmula from http://www.rapidtables.com/convert/color/rgb-to-cmyk.htm
//Work according to calculator at same site
sf::Vector3f BulbMath::rgb2cmyk(float r, float g, float b)
{

	float rr, gg, bb, C, M, Y, K;

	rr = r / 255.0f;
	gg = g / 255.0f;
	bb = b / 255.0f;

	K = 1 - (std::max(std::max(rr, gg), std::max(gg, bb)));
	C = (1 - rr - K) / (1 - K);
	M = (1 - gg - K) / (1 - K);
	Y = (1 - bb - K) / (1 - K);

	std::cout << "\nC: " << C << " M: " << M << " Y: " << Y << " K: " << K << std::endl;

	sf::Vector3f CMY(C, M, Y);

	return CMY;
}