#include <math.h>
#include "image.h"
#include <stdio.h>

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{

    image resized = make_image(w, h, im.c);
    float A = ((float)im.w / (float)resized.w);
    float b = - 0.5 + A * 0.5;


    for (int row =0; row< resized.h; row++)
    {
        for (int col=0; col < resized.w; col++)
        {
            float old_x = A* col + b;
            float old_y = A*row + b;

            for (int c_num =0; c_num < resized.c; c_num++)
            {
                set_pixel(resized, col, row, c_num, nn_interpolate(im, old_x, old_y, c_num));
            }
        }
    }
    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float x_min = floorf(x);
    float x_max = ceilf(x);

    float y_min = floorf(y);
    float y_max = ceilf(y);

    float q1 = 0.0;
    float q2 = 0.0;

    float q = 0.0;

    float w1_y = fabs(y - y_min);
    float w2_y = fabs(y - y_max);

    float w1_x = fabs(x - x_min);
    float w2_x = fabs(x - x_max);

    q1 = w1_y * get_pixel(im, x_min, y_max, c) + w2_y * get_pixel(im, x_min, y_min, c);
    q2 = w1_y * get_pixel(im, x_max, y_max, c) + w2_y * get_pixel(im, x_max, y_min, c);

    q = w1_x * q2 + w2_x * q1;

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);
    float A = ((float)im.w / (float)resized.w);
    float b = - 0.5 + A * 0.5;


    for (int row =0; row< resized.h; row++)
    {
        for (int col=0; col < resized.w; col++)
        {
            float old_x = A * col + b;
            float old_y = A * row + b;

            for (int c_num =0; c_num < resized.c; c_num++)
            {
                set_pixel(resized, col, row, c_num, bilinear_interpolate(im, old_x, old_y, c_num));
            }
        }
    }
    return resized;
}

