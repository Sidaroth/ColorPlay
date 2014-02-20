#include "BulbMath.hpp"

BulbMath::BulbMath()
{

}

//Makes sure rgb is within range 0 - 255
sf::Vector3f BulbMath::rgbThresholdCheck(sf::Vector3f rgb)
{
	if (rgb.x < 0)
	{
		rgb.x = 0.0f;
	}
	else if (rgb.x > 255)
	{
		rgb.x = 255.0f;
	}

	if (rgb.y < 0)
	{
		rgb.y = 0.0f;
	}
	else if (rgb.y > 255)
	{
		rgb.y = 255.0f;
	}

	if (rgb.z < 0)
	{
		rgb.z = 0.0f;
	}
	else if (rgb.z > 255)
	{
		rgb.z = 255.0f;
	}
	return rgb;
}

//Makes sure XYZ is within d(65) tristimulus reference white values
sf::Vector3f BulbMath::xyzThresholdCheck(sf::Vector3f xyz)
{
	if (xyz.x < 0)
	{
		xyz.x = 0.0f;
	}
	else if (xyz.x > 95.047)
	{
		xyz.x = 95.047f;
	}

	if (xyz.y < 0)
	{
		xyz.y = 0.0f;
	}
	else if (xyz.y > 100)
	{
		xyz.y = 100.0f;
	}

	if (xyz.z < 0)
	{
		xyz.z = 0.0f;
	}
	else if (xyz.x > 108.883)
	{
		xyz.z = 108.883f;
	}

	return xyz;
}

//Threshold check for hsv. H in 0 - 360, s in 0 - 100, v is 0.5 - 100. v is kept at 0.5 to prevent the bulb from turning off
sf::Vector3f BulbMath::hsvThresholdCheck(sf::Vector3f hsv)
{
	if (hsv.x < 0)
	{
		hsv.x = 0.0f;
	}
	else if (hsv.x > 360)
	{
		hsv.x = 360.0f;
	}

	if (hsv.y < 0)
	{
		hsv.y = 0.0f;
	}
	else if (hsv.y > 100)
	{
		hsv.y = 100.0f;
	}

	if (hsv.z < 0.5)
	{
		hsv.z = 0.5f;
	}
	else if (hsv.z > 100)
	{
		hsv.z = 100.0f;
	}

	return hsv;
}

//Threshold for lab, L in 0 - 100, a and b in -180 - 180
sf::Vector3f BulbMath::labThresholdCheck(sf::Vector3f lab)
{
	if (lab.x < 0)
	{
		lab.x = 0.0f;
	}
	else if (lab.x > 100)
	{
		lab.x = 100.0f;
	}

	if (lab.y < -180)
	{
		lab.y = -180.0f;
	}
	else if (lab.y > 180)
	{
		lab.y = 180.0f;
	}

	if (lab.z < -180)
	{
		lab.z = -180.0f;
	}
	else if (lab.z > 180)
	{
		lab.z = 180.0f;
	}
	return lab;
}

//Threshold check for cmy in range 0 - 1
sf::Vector3f BulbMath::cmyThresholdCheck(sf::Vector3f cmy)
{
	if (cmy.x < 0)
	{
		cmy.x = 0.0f;
	}
	else if (cmy.x > 1)
	{
		cmy.x = 1.0f;
	}

	if (cmy.y < 0)
	{
		cmy.y = 0.0f;
	}
	else if (cmy.y > 1)
	{
		cmy.y = 1.0f;
	}

	if (cmy.z < 0)
	{
		cmy.z = 0.0f;
	}
	else if (cmy.z > 1)
	{
		cmy.z = 1.0f;
	}
	return cmy;
}

//Returns XYZ in range 0 - 100. input, L in range 0 - 100, a and b in range -128 - 128
//Tested - Works according to calculator at http://www.brucelindbloom.com/
//remember to set reference white to D65 and rgb space to sRGB when using calculator
sf::Vector3f BulbMath::lab2xyz(float L, float a, float b)
{
	//Vector XYZ is first used for LAB threshold checking.
	sf::Vector3f XYZ(L, a, b);

	XYZ = labThresholdCheck(XYZ);

	//Tristimulus values of reference white D(65)
	float Xn = 95.047/100.0f;
	float Yn = 100.0/100.0f;
	float Zn = 108.883/100.0f;
	float fx, fy, fz, xr, yr, zr, X, Y, Z;

	//Tresholds
	float T1 = 0.008856;
	float T2 = 903.3;

	fy = (XYZ.x + 16.0f) / 116.0f;
	fz = fy - XYZ.z / 200.0f;
	fx = XYZ.y / 500.0f + fy;

	if (pow(fx, 3) > T1)
	{
		xr = pow(fx, 3);
	}
	else
	{
		xr = (116.0f * fx - 16.0f) / T2;
	}

	if (L > (T1 * T2))
	{
		yr = pow(((XYZ.x + 16.0f) / 116.0f), 3);
	}
	else
	{
		yr = XYZ.x / T2;
	}

	if (pow (fz, 3) > T1)
	{
		zr = pow(fz, 3);
	}
	else
	{
		zr = (116.0f * fz - 16.0f) / T2;
	}
	X = xr * Xn;
	Y = yr * Yn;
	Z = zr * Zn;

	XYZ.x = X * 100.0f;
	XYZ.y = Y * 100.0f;
	XYZ.z = Z * 100.0f;

	XYZ = xyzThresholdCheck(XYZ);


	//std::cout << "X: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

	return XYZ;
}


//input XYZ in rage 0 - 100, output L in range 0 - 100, a and b in range -180 - 180
//Tested - works according to bruce lindbloom calculator
sf::Vector3f BulbMath::xyz2lab(float X, float Y, float Z)
{
	float xr, yr, zr, fx, fy, fz, L, a, b;

	sf::Vector3f LAB(X, Y, Z);

	//LAB vector is first used for XYZ threshold check
	LAB = xyzThresholdCheck(LAB);

	//D65 Tristimulus values
	float Xn = 95.047;
	float Yn = 100.0;
	float Zn = 108.883;

	//Treshold
	float T1 = 0.008856;
	float T2 = 903.3;

	xr = LAB.x / Xn;
	yr = LAB.y / Yn;
	zr = LAB.z / Zn;

	if (xr > T1)
	{
		fx = cbrt(xr);
	}
	else
	{
		fx = (T2 * xr + 16.0f) / 116.0f;
	}

	if (yr > T1)
	{
		fy = cbrt(yr);
	}
	else
	{
		fy = (T2 * yr + 16.0f) / 116.0f;
	}

	if (zr > T1)
	{
		fz = cbrt(zr);
	}
	else
	{
		fz = (T2 * zr + 16.0f) / 116.0f;
	}

	L = 116.0f * fy - 16.0f;
	a = 500.0f * (fx - fy);
	b = 200.0f * (fy - fz);

	LAB.x = L;
	LAB.y = a;
	LAB.z = b;

	LAB = labThresholdCheck(LAB);

//	std::cout << "\nL: " << LAB.x << " a: " << LAB.y << " b: " << LAB.z << std::endl;

	return LAB;

}


//input XYZ in range 0 - 95.047, 0 - 100, 0 - 108.883, output rgb in range 0 - 255
//calculates rgb for d65 whitepoint and sRGB model, works according to bruce lindbloom calc
//tested 
sf::Vector3f BulbMath::xyz2rgb(float x, float y, float z)
{
	float var_X, var_Y, var_Z, var_R, var_G, var_B, R, G, B;

	sf::Vector3f RGB(x, y, z);

	//RGB vector is first used for xyz threshold check
	RGB = xyzThresholdCheck(RGB);

	var_X = RGB.x / 100;        //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	var_Y = RGB.y / 100;        //Y from 0 to 100.000
	var_Z = RGB.z / 100;        //Z from 0 to 108.883

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

	R = (var_R * 255);
	G = (var_G * 255);
	B = (var_B * 255);	

	RGB.x = R;
	RGB.y = G;
	RGB.z = B;

	RGB = rgbThresholdCheck(RGB);

//	std::cout << "\nR: " << R << " G: " << G << " B: " << B << std::endl;

	return RGB;
}

//input rgb in range 0 - 255, output XYZ in range 0 - 95.047, 0 - 100, 0 - 108.883. Works for d65 whitepoint and sRGB space
//NOTE, seems to calculate the wrong values sometimes when calculating outside the rgb breakpoints (under 0 or over 255) not sure if it is relevant as these values are clipped anyway
//Tested - sometimes returns vaules that are off by around 0.2, seems to be in the var > 0.04045 part of the ifs
sf::Vector3f BulbMath::rgb2xyz(float r, float g, float b)
{
	//TODO currently returns xyz rounded to 2 decimals, might need more accuracy.
	float var_R, var_G, var_B;

	sf::Vector3f XYZ(r, g, b);

	//XYZ vector is first used for rgb threshold check
	XYZ = rgbThresholdCheck(XYZ);

	var_R = ( XYZ.x / 255.0f );        //R from 0 to 255
	var_G = ( XYZ.y / 255.0f );        //G from 0 to 255
	var_B = ( XYZ.z / 255.0f );        //B from 0 to 255

	if ( var_R > 0.04045 )
	{
		var_R = ( pow((( var_R + 0.055 ) / 1.055 ), 2.4));
	} 
	else                   
	{
		var_R = var_R / 12.92;
	}
	if ( var_G > 0.04045 )
	{
		var_G = ( pow((( var_G + 0.055 ) / 1.055 ), 2.4));
	} 
	else
	{
		var_G = var_G / 12.92;
	}                   
	if ( var_B > 0.04045 )
	{
		var_B = ( pow((( var_B + 0.055 ) / 1.055 ), 2.4));
	} 
	else
	{
		var_B = var_B / 12.92;
	}                   

	var_R = var_R * 100.0f;
	var_G = var_G * 100.0f;
	var_B = var_B * 100.0f;

	//Observer. = 2°, Illuminant = D65
	XYZ.x = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	XYZ.y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	XYZ.z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	XYZ = xyzThresholdCheck(XYZ);

//	std::cout << "\nX: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

	return XYZ;
}


//Input = rgb in range 0 - 255, output = H in range 0 - 360, s and v in range 0 - 100
//Tested - works according to http://www.javascripter.net/faq/rgb2hsv.htm (only does hue to one decimal)
sf::Vector3f BulbMath::rgb2hsv(float r, float g, float b)
{
	sf::Vector3f hsv(r, g, b);

	//hsv vector is first used for rgb threshold check
	hsv = rgbThresholdCheck(hsv);

	float min, max, delta, rr, gg, bb;

	rr = hsv.x/255.0f;
	gg = hsv.y/255.0f;
	bb = hsv.z/255.0f;

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
		hsv.x = 0;		
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

	hsv.x = hsv.x * 60.0f;

	if (hsv.x < 0)
	{
		hsv.x = hsv.x + 360.0f;
	}

	hsv.y = hsv.y * 100.0f;
	hsv.z = hsv.z * 100.0f;

	hsv = hsvThresholdCheck(hsv);

	//Recalculating hsv to bulb range
	hsv.x = hsv.x * 182.04167;
	hsv.y = (hsv.y / 100) * 255;
	hsv.z = (hsv.z / 100) * 255;


	hsv.x = round(hsv.x);
	hsv.y = round(hsv.y);
	hsv.z = round(hsv.z);

	//std::cout << "\n RGB2HSV out------->> H: " << hsv.x << " s: " << hsv.y << " v: " << hsv.z << std::endl;

	return hsv;
}

//H in value 0 - 360, s and v in range 0 - 100; return rgb in range 0 - 255.
//Tested and works according to calculator @ http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
//However the calculators online rounds to rgb by flooring, so the exact accuracy of the function is unknown
sf::Vector3f BulbMath::hsv2rgb(float H, float s, float v)
{
	sf::Vector3f RGB(H, s, v);

	float f, p, q, t, hh, ss, vv;
	int i;

	//s and v is redefined into 0 - 100 range, H is redefined into 0 - 360 range.
	RGB.y = ((RGB.y / 255.0f) * 100.0f);
	RGB.z = ((RGB.z / 255.0f) * 100.0f);
	RGB.x = RGB.x / 182.04167;

	//rgb vector first used for hsv threshold check
	RGB = hsvThresholdCheck(RGB);

	ss = RGB.y / 100.0f;
	vv = RGB.z / 100.0f;

	if (ss == 0)		//This means the color is grey
	{
		RGB.x = RGB.y = RGB.z = vv;
		RGB.x = RGB.x * 255.0f;
		RGB.y = RGB.y * 255.0f;
		RGB.z = RGB.z * 255.0f;

		//std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;
		return RGB;
	}

	hh = RGB.x / 60.0f;
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

	RGB.x = (RGB.x * 255.0f);
	RGB.y = (RGB.y * 255.0f);
	RGB.z = (RGB.z * 255.0f);

	RGB = rgbThresholdCheck(RGB);

//	std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;
	return RGB;
}

//input cmyk in rage 0 - 1, output rgb in range 0 - 255
//math from http://www.rapidtables.com/convert/color/cmyk-to-rgb.htm, verified using calculator from same site
sf::Vector3f BulbMath::cmyk2rgb(float c, float m, float y, float k)
{
	sf::Vector3f RGB(c, m, y);

	//rgb vector first used for cmy threshold check
	RGB = cmyThresholdCheck(RGB);

	float kk = k;

	if (k < 0)
	{
		kk = 0.0f;
	}
	else if (k > 1)
	{
		kk = 1.0f;
	}

	RGB.x = 255 * (1 - RGB.x) * (1 - kk);
	RGB.y = 255 * (1 - RGB.y) * (1 - kk);
	RGB.z = 255 * (1 - RGB.z) * (1 - kk);

	RGB = rgbThresholdCheck(RGB);

	//std::cout << "\nR: " << RGB.x << " G: " << RGB.y << " B: " << RGB.z << std::endl;

	return RGB;
}

//input rgb in rage 0 - 255,output cymk in range 0 - 1
//note that functions returning cmyk values return a pointer to a float array with cmyk values in order
//math from http://www.rapidtables.com/convert/color/cmyk-to-rgb.htm, verified using calculator from same site
float * BulbMath::rgb2cmyk(float r, float g, float b)
{

	float rr, gg, bb, K;

	sf::Vector3f CMY(r, g, b);

	//cmy vector first used for rgb threshold check
	CMY = rgbThresholdCheck(CMY);

	rr = CMY.x / 255.0f;
	gg = CMY.y / 255.0f;
	bb = CMY.z / 255.0f;

	K = 1 - (std::max(std::max(rr, gg), std::max(gg, bb)));
	CMY.x = (1 - rr - K) / (1 - K);
	CMY.y = (1 - gg - K) / (1 - K);
	CMY.z = (1 - bb - K) / (1 - K);

//	std::cout << "\nC: " << CMY.x << " M: " << CMY.y << " Y: " << CMY.z << " K: " << K << std::endl;

	CMY = cmyThresholdCheck(CMY);

	if (K < 0)
	{
		K = 0.0;
	}
	else if (K > 1)
	{
		K = 1.0f;
	}

	static float CMYarray[4];
	CMYarray[0] = CMY.x;
	CMYarray[1] = CMY.y;
	CMYarray[2] = CMY.z;
	CMYarray[3] = K;

	return CMYarray;
}

//works according to http://colormine.org/convert/lab-to-hsv
sf::Vector3f BulbMath::hsv2lab(float h, float s, float v)
{
	sf::Vector3f Lab;
	Lab = hsv2rgb(h, s, v);
	Lab = rgb2xyz(Lab.x, Lab.y, Lab.z);
	Lab = xyz2lab(Lab.x, Lab.y, Lab.z);

//	std::cout << "\nL: " << Lab.x << " a: " << Lab.y << " b: " << Lab.z << std::endl;

	return Lab;
}

//s and v values are off by about 0.001 according to calulator, check it out
sf::Vector3f BulbMath::lab2hsv(float L, float a, float b)
{
	sf::Vector3f HSV;
	HSV = lab2xyz(L, a, b);
	HSV = xyz2rgb(HSV.x, HSV.y, HSV.z);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

//	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}

//Works http://www.rapidtables.com/convert/color/rgb-to-cmyk.htm
//note that functions returning cmyk values return a pointer to a float array with cmyk values in order
float * BulbMath::hsv2cmyk(float h, float s, float v)
{
	float *CMYarray;
	sf::Vector3f CMY;
	CMY = hsv2rgb(h, s, v);
	CMYarray = rgb2cmyk(CMY.x, CMY.y, CMY.z);

//	std::cout << "\nC: " << CMY.x << " M: " << CMY.y << " Y: " << CMY.z << std::endl;

	return CMYarray;
}

//A little off compaired to http://www.rapidtables.com/convert/color/rgb-to-hsv.htm, might be the calculator doing rounding, look for another one to compair
sf::Vector3f BulbMath::cmyk2hsv(float c, float m, float y, float k)
{
	sf::Vector3f HSV;
	HSV = cmyk2rgb(c, m ,y, k);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

//	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}

//Works according to http://colormine.org/convert/xyz-to-hsv
sf::Vector3f BulbMath::hsv2xyz(float h, float s, float v)
{
	sf::Vector3f XYZ;
	XYZ = hsv2rgb(h, s, v);
	XYZ = rgb2xyz(XYZ.x, XYZ.y, XYZ.z);

//	std::cout << "\nX: " << XYZ.x << " Y: " << XYZ.y << " Z: " << XYZ.z << std::endl;

	return XYZ;
}

sf::Vector3f BulbMath::xyz2hsv(float x, float y, float z)
{
	sf::Vector3f HSV;
	HSV = xyz2rgb(x, y, z);
	HSV = rgb2hsv(HSV.x, HSV.y, HSV.z);

//	std::cout << "\nH: " << HSV.x << " S: " << HSV.y << " V: " << HSV.z << std::endl;

	return HSV;
}