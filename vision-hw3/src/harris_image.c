#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, x+i, y, 0, 1);
        set_pixel(im, x, y+i, 0, 1);
        set_pixel(im, x+i, y, 1, 0);
        set_pixel(im, x, y+i, 1, 0);
        set_pixel(im, x+i, y, 2, 1);
        set_pixel(im, x, y+i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    // TODO: optional, make separable 1d Gaussian.
    int filter_size = (int)6.0f * sigma;
    filter_size = (filter_size % 2 == 0)? filter_size + 1 : filter_size;

    image gaussian_filter = make_image(filter_size, 1, 1);
    //float sum =0.0;

        for(int col=0; col < gaussian_filter.w; col++)
        {
            float x = (float)(col - gaussian_filter.w / 2);
            float value = expf(-(x*x)/ (2* sigma*sigma)) / (TWOPI * sigma * sigma);
            set_pixel(gaussian_filter, col, 0, 0, value);
        }
    //l1_normalize(gaussian_filter); //normalization not required here
    //printf("%f", sum);
    l1_normalize(gaussian_filter);
    return gaussian_filter;
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    if(0){
        image g = make_gaussian_filter(sigma);
        image s = convolve_image(im, g, 1);
        free_image(g);
        return s;
    } else {
        // TODO: optional, use two convolutions with 1d gaussian filter.
        // If you implement, disable the above if check.
        image g_row = make_1d_gaussian(sigma);
        image s = convolve_image(im, g_row, 1);

        image g_col = make_image(g_row.h, g_row.w, g_row.c);

        for (int i =0; i < g_col.h; i++)
        {
            set_pixel(g_col, 0, i, 0, get_pixel(g_row, i, 0, 0));
            //printf("%f --------  %f\n", get_pixel(g_row, i, 0, 0), get_pixel(g_col,0,i,0));
        }

        s = convolve_image(s, g_col, 1);
        return s;
    }
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{
    image S = make_image(im.w, im.h, 3);
    // TODO: calculate structure matrix for im.
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    image image_x = convolve_image(im, gx_filter, 0);
    image image_y = convolve_image(im, gy_filter, 0);


    for (int y = 0; y < S.h; y++)
    {
        for (int x =0; x < S.w; x++)
        {
             set_pixel(S, x, y, 0, get_pixel(image_x, x, y, 0) * get_pixel(image_x, x, y, 0));
             set_pixel(S, x, y, 1, get_pixel(image_y, x, y, 0) * get_pixel(image_y, x, y, 0));
             set_pixel(S, x, y, 2, get_pixel(image_x, x, y, 0) * get_pixel(image_y, x, y, 0));


        }
    }

    S = smooth_image(S, sigma);
    free_image(gx_filter);
    free_image(gy_filter);
    free_image(image_x);
    free_image(image_y);
    return S;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S)
{
    image R = make_image(S.w, S.h, 1);
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    float I_x_2 = 0.0;
    float I_x_y = 0.0;
    float I_y_2 = 0.0;
    float det_S = 0.0;
    float trace_S = 0.0;
    float cornerness_pixel_val = 0.0;
    for (int y = 0; y < R.h; y++)
    {
        for (int x =0; x < R.w; x++)
        {
            I_x_2 = get_pixel(S, x, y, 0);
            I_y_2 = get_pixel(S, x, y, 1);
            I_x_y = get_pixel(S, x, y, 2);
            det_S = I_x_2 * I_y_2 - I_x_y * I_x_y;
            trace_S = I_x_2 + I_y_2;
            cornerness_pixel_val = det_S - 0.06 * trace_S * trace_S;
            set_pixel(R, x, y, 0, cornerness_pixel_val);

        }
    }
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    int window_size = 2*w + 1;
    image r = copy_image(im);
    // TODO: perform NMS on the response map.
    float curr_pixel_val =0.0;
    float neighbor_pixel_val = 0.0;

    for (int y =0; y < im.h; y++)
    {
        for (int x =0; x < im.w ; x++)
        {
            curr_pixel_val = get_pixel(im , x, y, 0);
            for (int w_y = 0; w_y < window_size; w_y++)
            {
                for (int w_x =0; w_x < window_size; w_x++)
                {
                    neighbor_pixel_val = get_pixel(im, x + w_x - window_size /2, y + w_y - window_size /2, 0);
                    if (neighbor_pixel_val > curr_pixel_val)
                    {
                        set_pixel(r, x, y, 0, -999999);
                        // once set to low response we don't  need to check other remaining neighbours, if any
                        w_x = window_size;
                        w_y = window_size;

                    }


                }
            }
        }
    }
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);

    int count =0;
    //TODO: count number of responses over threshold
    for (int y =0; y < Rnms.h; y++)
    {
        for (int x =0; x < Rnms.w; x++)
        {
            if (get_pixel(Rnms, x, y, 0) > thresh)
                count++;
        }
    }

    
    *n = count; // <- set *n equal to number of corners in image.
    descriptor *d = calloc(count, sizeof(descriptor));
    //TODO: fill in array *d with descriptors of corners, use describe_index.

    int descriptor_count =0;

    for (int y =0; y < Rnms.h; y ++) {
        for (int x = 0; x < Rnms.w; x++)
        {
            if ((descriptor_count <= count) && (get_pixel(Rnms, x, y, 0) > thresh))
            {
                d[descriptor_count] = describe_index(im, x + Rnms.w * y);
                descriptor_count++;

            }

        }
    }

    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
