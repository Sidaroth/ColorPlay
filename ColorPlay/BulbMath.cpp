#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

//Makes sure rgb is within range
float BulbMath::rgbThresholdCheck(float x)
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

//Makes sure XYZ is within range
float BulbMath::xyzThresholdCheck(float x)
{
	if (x < 0)
	{
		return 0.0f;
	}
	else if (x > 100)
	{
		return 100.0f;
	}
	else
	{
		return x;
	}
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
		std::cout << "\n- - - - - - - - - - STEP 1 - - - - - - - - - -" << std::endl;
		xr = pow(fx, 3);
	}
	else
	{
		std::cout << "\n- - - - - - - - - - STEP 2 - - - - - - - - - -" << std::endl;
		xr = (116 * fx - 16) / T2;
	}

	if (L > (T1 * T2))
	{
		std::cout << "\n- - - - - - - - - - STEP 3 - - - - - - - - - -" << std::endl;
		yr = pow(((L + 16) / 116), 3);
	}
	else
	{
		std::cout << "\n- - - - - - - - - - STEP 4 - - - - - - - - - -" << std::endl;
		yr = L / T2;
	}

	if (pow (fz, 3) > T1)
	{
		std::cout << "\n- - - - - - - - - - STEP 5 - - - - - - - - - -" << std::endl;
		zr = pow(fz, 3);
	}
	else
	{
		std::cout << "\n- - - - - - - - - - STEP 6 - - - - - - - - - -" << std::endl;
		zr = (116 * fz - 16) / T2;
	}
	X = xr * Xn;
	Y = yr * Yn;
	Z = zr * Zn;

	X = xyzThresholdCheck((X*100));
	Y = xyzThresholdCheck((Y*100));
	Z = xyzThresholdCheck((Z*100));



	sf::Vector3f XYZ(X, Y, Z);
	std::cout << "X: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

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
	//std::cout << "\nL: " << L << " a: " << a << " b: " << b << std::endl;

	return LAB;

}


//input XYZ in range 0 - 95.047, 0 - 100, 0 - 108.883, output rgb in range 0 - 255
//calculates rgb for d65 whitepoint and sRGB model, works according to bruce lindbloom calc
//tested 
sf::Vector3f BulbMath::xyz2rgb(float x, float y, float z)
{
	float var_X, var_Y, var_Z, var_R, var_G, var_B, R, G, B;

	var_X = x / 100;        //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	var_Y = y / 100;        //Y from 0 to 100.000
	var_Z = z / 100;        //Z from 0 to 108.883

	var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
	var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
	var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

	if ( var_R > 0.0031308 ) 
		var_R = 1.055 * ( pow(var_R, (1/2.4)) ) - 0.055;
	else                     
		var_R = 12.92 * var_R;
	if ( var_G > 0.0031308 ) 
		var_G = 1.055 * ( pow(var_G, (1/2.4)) ) - 0.055;
	else                     
		var_G = 12.92 * var_G;
	if ( var_B > 0.0031308 ) 
		var_B = 1.055 * ( pow(var_B, (1/2.4)) ) - 0.055;
	else                     
		var_B = 12.92 * var_B;

	std::cout << "\nvR: " << var_R << " vG: " << var_G << " vB: " << var_B << std::endl;

	var_R = rgbThresholdCheck(var_R);
	var_G = rgbThresholdCheck(var_G);
	var_B = rgbThresholdCheck(var_B);

	//Not sure if these values should be rounded
	R = (var_R * 255);
	G = (var_G * 255);
	B = (var_B * 255);	

	sf::Vector3f RGB(R, G, B);

	std::cout << "\nR: " << R << " G: " << G << " B: " << B << std::endl;

	return RGB;
}

//input rgb in range 0 - 255, output XYZ in range 0 - 95.047, 0 - 100, 0 - 108.883. Works for d65 whitepoint and sRGB space
//NOTE, seems to calculate the wrong values sometimes when calculating outside the rgb breakpoints (under 0 or over 255) not sure if it is relevant as these values are clipped anyway
sf::Vector3f BulbMath::rgb2xyz(float r, float g, float b)
{
	//TODO currently returns xyz rounded to 2 decimals, might need more accuracy.
	float var_R, var_G, var_B, X, Y, Z;

	var_R = ( r / 255 );        //R from 0 to 255
	var_G = ( g / 255 );        //G from 0 to 255
	var_B = ( b / 255 );        //B from 0 to 255

	if ( var_R > 0.04045 ) 
		var_R = ( pow((( var_R + 0.055 ) / 1.055 ), 2.4));
	else                   
		var_R = var_R / 12.92;
	if ( var_G > 0.04045 ) 
		var_G = ( pow((( var_G + 0.055 ) / 1.055 ), 2.4));
	else                   
		var_G = var_G / 12.92;
	if ( var_B > 0.04045 ) 
		var_B = ( pow((( var_B + 0.055 ) / 1.055 ), 2.4));
	else                   
		var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	//Observer. = 2°, Illuminant = D65
	X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	std::cout << "\nX: " << X << " Y: " << Y << " Z: " << Z << std::endl;

	sf::Vector3f XYZ(X, Y, Z);

	return XYZ;
}


//Input = rgb in range 0 - 255, output = H in range 0 - 360, s and v in range 0 - 100
//works according to http://www.rapidtables.com/convert/color/rgb-to-hsv.htm but not according to http://colormine.org/convert/xyz-to-hsv
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

	RGB.x = (RGB.x * 255);
	RGB.y = (RGB.y * 255);
	RGB.z = (RGB.z * 255);
	std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;
	return RGB;
}

//input cmyk in rage 0 - 1, output rgb in range 0 - 255
//math from http://www.rapidtables.com/convert/color/cmyk-to-rgb.htm, verified using calculator from same site
sf::Vector3f BulbMath::cmyk2rgb(float c, float m, float y, float k)
{
	float R, G, B;

	R = 255 * (1 - c) * (1 - k);
	G = 255 * (1 - m) * (1 - k);
	B = 255 * (1 - y) * (1 - k);

	sf::Vector3f RGB(R, G, B);

	std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;

	return RGB;
}

//input rgb in rage 0 - 255,output cymk in range 0 - 1
//math from http://www.rapidtables.com/convert/color/cmyk-to-rgb.htm, verified using calculator from same site
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

//works according to http://colormine.org/convert/lab-to-hsv
sf::Vector3f BulbMath::hsv2lab(float h, float s, float v)
{
	sf::Vector3f Lab;
	Lab = hsv2rgb(h, s, v);
	Lab = rgb2xyz(Lab.x, Lab.y, Lab.z);
	Lab = xyz2lab(Lab.x, Lab.y, Lab.z);

	std::cout << "\nL: " << Lab.x << " a: " << Lab.y << " b: " << Lab.z << std::endl;

	return Lab;
}

//s and v values are off by about 0.001 according to calulator, check it out
sf::Vector3f BulbMath::lab2hsv(float L, float a, float b)
{
	sf::Vector3f HSV;
	HSV = lab2xyz(L, a, b);
	HSV = xyz2rgb(HSV.x, HSV.y, HSV.z);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}

//Works http://www.rapidtables.com/convert/color/rgb-to-cmyk.htm
sf::Vector3f BulbMath::hsv2cmyk(float h, float s, float v)
{
	sf::Vector3f CMY;
	CMY = hsv2rgb(h, s, v);
	CMY = rgb2cmyk(CMY.x, CMY.y, CMY.z);

	std::cout << "\nC: " << CMY.x << " M: " << CMY.y << " Y: " << CMY.z << std::endl;

	return CMY;
}

//A little off compaired to http://www.rapidtables.com/convert/color/rgb-to-hsv.htm, might be the calculator doing rounding, look for another one to compair
sf::Vector3f BulbMath::cmyk2hsv(float c, float m, float y, float k)
{
	sf::Vector3f HSV;
	HSV = cmyk2rgb(c, m ,y, k);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}

//Works according to http://colormine.org/convert/xyz-to-hsv
sf::Vector3f BulbMath::hsv2xyz(float h, float s, float v)
{
	sf::Vector3f XYZ;
	XYZ = hsv2rgb(h, s, v);
	XYZ = rgb2xyz(XYZ.x, XYZ.y, XYZ.z);

	std::cout << "\nX: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

	return XYZ;
}

sf::Vector3f BulbMath::xyz2hsv(float x, float y, float z)
{
	sf::Vector3f HSV;
	HSV = xyz2rgb(x, y, z);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}