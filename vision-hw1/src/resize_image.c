#include <math.h>
#include "image.h"
#include <stdio.h>

#include <stdlib.h>


float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, (int)round(x), (int)round(y), c);
}

image nn_resize(image im, int w, int h)
{

    image resized = make_image(w, h, im.c);
    float A_w = ((float)im.w / (float)resized.w);
    float b_w = - 0.5 + A_w * 0.5;

    float A_h = ((float)im.h / (float)resized.h);
    float b_h = - 0.5 + A_h * 0.5;

    float old_x, old_y;
    for (int c_num =0; c_num < resized.c; c_num++)
    {
        for (int row =0; row< resized.h; row++)
        {
            for (int col=0; col < resized.w; col++)
            {
                old_x = A_w* col + b_w;
                old_y = A_h* row + b_h;
                set_pixel(resized, col, row, c_num, nn_interpolate(im, old_x, old_y, c_num));

            }
        }

    }

    return resized;
}


float bilinear_interpolate(image im, float x, float y, int c)
{
    int x_min = (int)floorf(x);
    int x_max = (int)ceilf(x);

    int y_min = (int)floorf(y);
    int y_max = (int)ceilf(y);

    float q1 = 0.0;
    float q2 = 0.0;

    float q = 0.0;
    
        // compute weights
/*
         w1_x     w2_x
        -----------------
        |      |        | w1_y
        |      |        |
      q1|------|--------| q2
        |      |        |
        |      |        | w2_y
        |      |        |
        -----------------
*/

    float w1_y = y - y_min;
    float w2_y = y_max -y;

    float w1_x = x - x_min;
    float w2_x = x_max - x;

    q1 = w1_y * get_pixel(im, x_min, y_max, c) + w2_y * get_pixel(im, x_min, y_min, c);
    q2 = w1_y * get_pixel(im, x_max, y_max, c) + w2_y * get_pixel(im, x_max, y_min, c);

    q = w1_x * q2 + w2_x * q1;

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);
    float A_w = ((float)im.w / (float)resized.w);
    float b_w = - 0.5 + A_w * 0.5;

    float A_h = ((float)im.h / (float)resized.h);
    float b_h = - 0.5 + A_h * 0.5;

    float old_x, old_y;
    for (int c_num =0; c_num < resized.c; c_num++)
    {
        for (int row =0; row< resized.h; row++)
        {
            for (int col=0; col < resized.w; col++)
            {
                old_x = A_w* col + b_w;
                old_y = A_h* row + b_h;
                set_pixel(resized, col, row, c_num, bilinear_interpolate(im, old_x, old_y, c_num));

            }
        }

    }
    return resized;
}
