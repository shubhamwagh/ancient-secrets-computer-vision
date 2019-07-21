#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for (int c =0; c< im.c; c++)
    {
        for (int row = 0; row < im.h; row++)
        {
            for (int col=0; col < im.w; col++)
            {
                set_pixel(im, col, row, c, get_pixel(im, col, row, c) / (im.w * im.h * 1.0));
            }
        }

    }

}

image make_box_filter(int w)
{
    image box_filter = make_image(w, w, 1);

    for (int c =0; c< box_filter.c; c++)
    {
        for (int row = 0; row < box_filter.h; row++)
        {
            for (int col=0; col < box_filter.w; col++)
            {
                set_pixel(box_filter, col, row, c, 1.0);
            }
        }

    }

    l1_normalize(box_filter);
    return box_filter;
}


image convolve_image(image im, image filter, int preserve)
{
    float new_pixel_val = 0.0;
    image convolved_image;
    assert(filter.c == im.c || filter.c == 1);

    if (preserve == 1)
    {
        convolved_image = make_image(im.w, im.h, im.c);
    }
     else
    {
        convolved_image = make_image(im.w, im.h, 1);
    }


     int filter_center = filter.w /2 ;

     for (int c = 0; c < im.c; c++)
     {
         int filter_channel = (filter.c == 1) ? 0 : c;
         for (int row = 0; row < im.h; row++)
         {
             for (int col = 0; col < im.w; col++)
             {
                 new_pixel_val = 0.0;
                 for (int filter_row= 0; filter_row < filter.h; filter_row++)
                 {
                     for (int filter_col = 0; filter_col < filter.w; filter_col++)
                     {


                         new_pixel_val += get_pixel(filter, filter_col, filter_row, filter_channel) *
                                          get_pixel(im, col + filter_col - filter_center, row + filter_row - filter_center, c);
                     }
                 }
                 if (preserve == 1)
                 {
                     set_pixel(convolved_image, col, row, c, new_pixel_val);
                 }
                 else
                 {
                     set_pixel(convolved_image, col, row, 0, new_pixel_val + get_pixel(convolved_image, col, row, 0));
                 }

             }
         }
     }

    return convolved_image;
}

image make_highpass_filter()
{

    image high_pass_filter = make_box_filter(3);

    set_pixel(high_pass_filter, 0, 0, 0, 0);
    set_pixel(high_pass_filter, 0, 1, 0, -1);
    set_pixel(high_pass_filter, 0, 2, 0, 0);

    set_pixel(high_pass_filter, 1, 0, 0, -1);
    set_pixel(high_pass_filter, 1, 1, 0, 4);
    set_pixel(high_pass_filter, 1, 2, 0, -1);

    set_pixel(high_pass_filter, 2, 0, 0, 0);
    set_pixel(high_pass_filter, 2, 1, 0, -1);
    set_pixel(high_pass_filter, 2, 2, 0, 0);

    return high_pass_filter;

}

image make_sharpen_filter()
{
    image sharpen_filter = make_box_filter(3);

    set_pixel(sharpen_filter, 0, 0, 0, 0);
    set_pixel(sharpen_filter, 0, 1, 0, -1);
    set_pixel(sharpen_filter, 0, 2, 0, 0);

    set_pixel(sharpen_filter, 1, 0, 0, -1);
    set_pixel(sharpen_filter, 1, 1, 0, 5);
    set_pixel(sharpen_filter, 1, 2, 0, -1);

    set_pixel(sharpen_filter, 2, 0, 0, 0);
    set_pixel(sharpen_filter, 2, 1, 0, -1);
    set_pixel(sharpen_filter, 2, 2, 0, 0);

    return sharpen_filter;
}

image make_emboss_filter()
{
    image emboss_filter = make_box_filter(3);


    set_pixel(emboss_filter, 0, 0, 0, -2.0f);
    set_pixel(emboss_filter, 0, 1, 0, -1.0f);
    set_pixel(emboss_filter, 0, 2, 0, 0.0f);

    set_pixel(emboss_filter, 1, 0, 0, -1.0f);
    set_pixel(emboss_filter, 1, 1, 0, 1.0f);
    set_pixel(emboss_filter, 1, 2, 0, 1.0f);

    set_pixel(emboss_filter, 2, 0, 0, 0.0f);
    set_pixel(emboss_filter, 2, 1, 0, 1.0f);
    set_pixel(emboss_filter, 2, 2, 0, 2.0f);

    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Highpass filter does not require to preserve channels.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Have to clamp images after convolutions.

image make_gaussian_filter(float sigma)
{
    int filter_size = (int)6.0f * sigma;
    filter_size = (filter_size % 2 == 0)? filter_size + 1 : filter_size;

    image gaussian_filter = make_image(filter_size, filter_size, 1);
    //float sum =0.0;
    for (int row =0; row < gaussian_filter.h; row++)
    {
        for(int col=0; col < gaussian_filter.w; col++)
        {
            float x = (float)(col - gaussian_filter.w / 2);
            float y = (float)(row - gaussian_filter.h / 2);
            float value = expf(-(x*x + y*y)/ (2* sigma*sigma)) / (TWOPI * sigma * sigma);
            set_pixel(gaussian_filter, col, row, 0, value);
        }
    }
    //l1_normalize(gaussian_filter); //normalization not required here
    //printf("%f", sum);
    return gaussian_filter;
}


image add_image(image a, image b)
{
    assert((a.w == b.w) && (a.h == b.h) && (a.c == b.c));
    image result_image = make_image(a.w, a.h, a.c);

    for (int c=0; c < a.c; c++)
    {
        for (int row=0; row< a.h; row++)
        {
            for (int col=0; col< a.w; col++)
            {
                set_pixel(result_image, col, row, c, get_pixel(a, col, row, c) + get_pixel(b, col, row, c));
            }
        }

    }

    return result_image;
}

image sub_image(image a, image b)
{
    assert((a.w == b.w) && (a.h == b.h) && (a.c == b.c));
    image result_image = make_image(a.w,a.h,a.c);

    for (int c=0; c < a.c; c++)
    {
        for (int row=0; row< a.h; row++)
        {
            for (int col=0; col< a.w; col++)
            {
                set_pixel(result_image, col, row, c, get_pixel(a, col, row, c) - get_pixel(b, col, row, c));
            }
        }

    }

    return result_image;
}

image make_gx_filter()
{
    image gx_filter = make_box_filter(3);

    set_pixel(gx_filter, 0, 0, 0, -1);
    set_pixel(gx_filter, 0, 1, 0, 0);
    set_pixel(gx_filter, 0, 2, 0, 1);

    set_pixel(gx_filter, 1, 0, 0, -2);
    set_pixel(gx_filter, 1, 1, 0, 0);
    set_pixel(gx_filter, 1, 2, 0, 2);

    set_pixel(gx_filter, 2, 0, 0, -1);
    set_pixel(gx_filter, 2, 1, 0, -0);
    set_pixel(gx_filter, 2, 2, 0, 1);

    return gx_filter;
}

image make_gy_filter()
{
    image gy_filter = make_box_filter(3);

    set_pixel(gy_filter, 0, 0, 0, -1);
    set_pixel(gy_filter, 0, 1, 0, -2);
    set_pixel(gy_filter, 0, 2, 0, 1);

    set_pixel(gy_filter, 1, 0, 0, 0);
    set_pixel(gy_filter, 1, 1, 0, 0);
    set_pixel(gy_filter, 1, 2, 0, 0);

    set_pixel(gy_filter, 2, 0, 0, 1);
    set_pixel(gy_filter, 2, 1, 0, 2);
    set_pixel(gy_filter, 2, 2, 0, 1);

    return gy_filter;
}

void feature_normalize(image im)
{
    float min =0.0;
    float max = 0.0;
    float pixel_val =0.0;
    for (int c=0; c < im.c; c++)
    {
        for (int row=0; row< im.h; row++)
        {
            for (int col=0; col< im.w; col++)
            {
                pixel_val = get_pixel(im, col, row, c);

                if (pixel_val > max)
                    max = pixel_val;

                if (pixel_val < min)
                    min = pixel_val;

            }
        }

    }

    float range = max - min;

    for (int c=0; c < im.c; c++)
    {
        for (int row=0; row< im.h; row++)
        {
            for (int col=0; col< im.w; col++)
            {
                if (range == 0)
                    set_pixel(im, col, row, c, 0.0);
                else
                    set_pixel(im, col, row, c, (get_pixel(im, col, row, c) - min )/ range);
            }
        }

    }
}



image *sobel_image(image im)
{
    // TODO
    image *sobel_result;
    sobel_result = calloc(2, sizeof(image));

    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    image convolved_gx_image = convolve_image(im, gx_filter, 0);
    image convolved_gy_image = convolve_image(im, gx_filter, 0);

    assert((convolved_gx_image.w == convolved_gy_image.w) && (convolved_gx_image.h == convolved_gy_image.h) && (convolved_gx_image.c == convolved_gy_image.c));

    image magnitude = make_image(im.w, im.h, 1);
    image gradient = make_image(im.w, im.h, 1);

    for (int row =0; row < im.h; row++)
    {
        for (int col =0; col < im.w; col++)
        {
            float gx = get_pixel(convolved_gx_image, col, row,0);
            float gy = get_pixel(convolved_gy_image, col, row, 0);

            float magnitude_val = sqrt(gx * gx + gy * gy);

            float gradient_val = atan2(gy, gx);

            set_pixel(magnitude, col, row, 0, magnitude_val);
            set_pixel(gradient, col, row, 0, gradient_val);
        }
    }

    sobel_result[0] = magnitude;
    sobel_result[1] = gradient;

    free_image(convolved_gx_image);
    free_image(convolved_gy_image);
    free_image(gx_filter);
    free_image(gy_filter);

    return sobel_result;
}


image colorize_sobel(image im)
{
    image filter = make_gaussian_filter(0.5);
    image blur_im = convolve_image(im, filter, 1);

    image im_colorize_sobel = make_image(blur_im.w, blur_im.h, blur_im.c);

    image *sobel = sobel_image(blur_im);
    feature_normalize(sobel[0]);
    feature_normalize(sobel[1]);

    for (int c=0; c < blur_im.c; c++)
    {
        for (int row=0; row< blur_im.h; row++)
        {
            for (int col=0; col< blur_im.w; col++)
            {
                if (c == 0)
                    set_pixel(im_colorize_sobel, col, row, c, get_pixel(sobel[1], col, row, 0));
                else
                    set_pixel(im_colorize_sobel, col, row, c, get_pixel(sobel[0], col, row, 0));
            }
        }

    }

    hsv_to_rgb(im_colorize_sobel);
    free_image (blur_im);
    free_image (filter);
    free(sobel);
    return im_colorize_sobel;

}


