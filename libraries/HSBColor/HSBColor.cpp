#include "HSBColor.h"


// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
float MIN(float a, float b){
	if(a <= b){
		return a;
	}else{
		return b;
	}
}
float MAX(float a, float b){
	if(a >= b){
		return a;
	}else{
		return b;
	}
}

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = MIN(MIN(r, g), b );
	max = MAX(MAX(r, g), b );
	*v = max;				// v

	delta = max - min;

	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if(delta == 0){
		*h = 0;
	}else	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;

}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}


/* HSB to RGB conversion function
	 INPUT PARAMETERS: Hue values should range from 0 to 360, Saturation and Brightness values should range from 0 to 100
					   Colors pointer should resolve to an array with 3 elements
	 RGB values are passed back using via the array. Each value will range between 0 and 255

*/
void H2R_HSBtoRGB(int hue, int sat, int bright, int* colors) {

	// constrain all input variables to expected range
    hue = constrain(hue, 0, 360);
    sat = constrain(sat, 0, 100);
    bright = constrain(bright, 0, 100);

	// define maximum value for RGB array elements
	float max_rgb_val = H2R_MAX_RGB_val;

	// convert saturation and brightness value to decimals and init r, g, b variables
    float sat_f = float(sat) / 100.0;
    float bright_f = float(bright) / 100.0;
    int r, g, b;
		float red_float, green_float, blue_float;
		HSVtoRGB(&red_float, &green_float, &blue_float, hue, sat_f, bright_f);

		colors[0] = red_float * max_rgb_val;
		colors[1] = green_float * max_rgb_val;
		colors[2] = blue_float * max_rgb_val;

}

void H2R_RGBtoHSB(int * rgb, int * hsb){
	float max_rgb_val = H2R_MAX_RGB_val;
	float r = float(rgb[0])/max_rgb_val;
	float g = float(rgb[1])/max_rgb_val;
	float b = float(rgb[2])/max_rgb_val;
	float h,s,v;

	RGBtoHSV(r, g, b, &h, &s, &v);

	hsb[0] = h;
	hsb[1] = s*100;
	hsb[2] = v*100;
}

void H2R_HSBtoRGBfloat(float hue, float sat, float bright, int* colors) {
	if (hue > 1) hue = 1.0;
	if (sat > 1) sat = 1.0;
	if (bright > 1) bright = 1.0;
    H2R_HSBtoRGB(hue*360.0, sat*100.0, bright*100.0, colors);
}


/* This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send
 a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */
