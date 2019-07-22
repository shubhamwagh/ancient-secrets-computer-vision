#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{

	x = x < 0 ? 0 : (x >= im.w ? im.w -1 : x);
	y = y < 0 ? 0 : (y >= im.h ? im.h -1 : y);
	c = c < 0 ? 0 : (c >= im.c ? im.c -1 : c);

	return im.data[x + y*im.w + c*im.w*im.h];

}

void set_pixel(image im, int x, int y, int c, float v)
{
	if ((x < 0 || x >= im.w) || (y < 0 || y >= im.h) || (c < 0 || c >= im.c))
		return;
	else
		im.data[x + y*im.w + c*im.w*im.h] = v;

}

image copy_image(image im)
{
	image copy = make_image(im.w, im.h, im.c);

	//for (int i=0; i < (im.w * im.h * im.c); i++) {
	//    copy.data[i] = im.data[i];
	//}
	for (int y =0; y < im.h; y++)
	{
		for (int x=0; x < im.w; x++)
		{

			set_pixel(copy, x, y, 0, get_pixel(im, x, y, 0));
			set_pixel(copy, x, y, 1, get_pixel(im, x, y, 1));
			set_pixel(copy, x, y, 2, get_pixel(im, x, y, 2));
		}
	}
	//memcpy(copy.data, im.data, sizeof(float)*im.w * im.h * im.c);
	return copy;
}

image rgb_to_grayscale(image im)
{
	assert(im.c == 3);
	image gray = make_image(im.w, im.h, 1);

	float L;
	for (int row = 0; row < im.h; row++) {
		for (int col = 0; col < im.w; col++) {
			// relative luminance
			L = 0.299 * get_pixel(im, col, row, 0) + 0.587 * get_pixel(im, col, row, 1) + 0.114 * get_pixel(im, col, row, 2);

			set_pixel(gray, col, row, 0, L);
		}
	}
	return gray;
}

void shift_image(image im, int c, float v)
{
	for (int y =0; y < im.h; y++)
	{
		for (int x=0; x < im.w; x++)
		{
			set_pixel(im, x, y, c, get_pixel(im, x, y, c) + v);
		}
	}
}

void clamp_image(image im)
{

	for (int c =0; c < im.c; c++)
	{
		for (int y =0; y < im.h; y++)
		{
			for (int x=0; x < im.w; x++)
			{

				float v = get_pixel(im, x, y, c);
				v = (v < 0.0)? 0.0 : (v > 1.0 ? 1.0 : v);
				set_pixel(im, x, y, c, v);

			}
		}

	}

}


// These might be handy
float three_way_max(float a, float b, float c)
{
	return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
	return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{

	float red_channel_pixel_val = 0.0;
	float green_channel_pixel_val = 0.0;
	float blue_channel_pixel_val = 0.0;
	float hue = 0.0;
	float saturation = 0.0;
	float value = 0.0;
	float C = 0.0;
	float min = 0.0;


	for (int y=0; y < im.h; y++)
	{
		for (int x =0 ; x < im.w; x++)
		{
			red_channel_pixel_val = get_pixel(im, x, y, 0);
			green_channel_pixel_val = get_pixel(im, x, y, 1);
			blue_channel_pixel_val = get_pixel(im, x, y, 2);

			value = three_way_max(red_channel_pixel_val, green_channel_pixel_val, blue_channel_pixel_val);

			min = three_way_min(red_channel_pixel_val, green_channel_pixel_val, blue_channel_pixel_val);
			C = value - min;


			if (value!=0)
			{
				saturation = C / value;
			}
			else saturation = 0;



			if (C == 0)
			{
				hue = 0;
			}
			else if (value == red_channel_pixel_val )
			{
				hue = (green_channel_pixel_val - blue_channel_pixel_val) / C;
			}
			else if (value == green_channel_pixel_val)
			{
				hue = (blue_channel_pixel_val - red_channel_pixel_val) / C + 2;
			}
			else
			{
				hue = (red_channel_pixel_val - green_channel_pixel_val)/ C + 4;
			}


			if (hue < 0)
			{
				hue = hue/6 + 1;

			}
			else
			{
				hue = hue/6;
			}

			set_pixel(im, x, y, 0, hue);
			set_pixel(im, x, y, 1, saturation);
			set_pixel(im, x, y, 2, value);
		}
	}
}


void hsv_to_rgb(image im)
{

	float red_channel_pixel_val = 0.0;
	float green_channel_pixel_val = 0.0;
	float blue_channel_pixel_val = 0.0;
	float hue = 0.0;
	float hue_prime = 0.0;
	float saturation = 0.0;
	float value = 0.0;
	float C = 0.0;
	float min = 0.0;
	float X = 0.0;

	for (int y=0; y < im.h; y++)
	{
		for (int x =0 ; x < im.w; x++)
		{
			hue = get_pixel(im, x, y, 0);
			saturation = get_pixel(im, x, y, 1);
			value = get_pixel(im, x, y, 2);

			C = saturation * value;
			min = value - C;

			hue_prime = hue *  6;

			X = C * (1 - fabs(fmod(hue_prime, 2) - 1));

			if (hue_prime >=0 && hue_prime < 1)
			{
				red_channel_pixel_val = C;
				green_channel_pixel_val = X;
				blue_channel_pixel_val = 0;
			}

			else if (hue_prime >=1 && hue_prime < 2)
			{
				red_channel_pixel_val = X;
				green_channel_pixel_val = C;
				blue_channel_pixel_val = 0;
			}

			else if (hue_prime >=2 && hue_prime < 3)
			{
				red_channel_pixel_val = 0;
				green_channel_pixel_val = C;
				blue_channel_pixel_val = X;
			}

			else if (hue_prime >=3 && hue_prime < 4)
			{
				red_channel_pixel_val = 0;
				green_channel_pixel_val = X;
				blue_channel_pixel_val = C;
			}

			else if (hue_prime >=4 && hue_prime < 5)
			{
				red_channel_pixel_val = X;
				green_channel_pixel_val = 0;
				blue_channel_pixel_val = C;
			}

			else if (hue_prime >=5 && hue_prime < 6)
			{
				red_channel_pixel_val = C;
				green_channel_pixel_val = 0;
				blue_channel_pixel_val = X;
			}

			else
			{
				red_channel_pixel_val = min;
				green_channel_pixel_val = min;
				blue_channel_pixel_val = min;
			}

			red_channel_pixel_val += min;
			green_channel_pixel_val += min;
			blue_channel_pixel_val += min;

			set_pixel(im, x, y, 0, red_channel_pixel_val);
			set_pixel(im, x, y, 1, green_channel_pixel_val);
			set_pixel(im, x, y, 2, blue_channel_pixel_val);



		}
	}

}

void scale_image(image im, int c, float v)
{
	float new_pixel_value = 0.0;

	for (int y =0; y < im.h; y++)
	{
		for (int x=0; x < im.w; x++)
		{
			new_pixel_value = get_pixel(im, x, y, c) * v;
			set_pixel(im, x, y, c, new_pixel_value);
		}
	}
}



