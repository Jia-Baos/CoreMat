//
// Created by Jia-Baos on 2023/9/22.
//
#pragma
#ifndef SRC_IMAGE_HPP
#define SRC_IMAGE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/********** STRUCTURES *********/

/* structure for 1-channel image */
struct gray_image {
  int width;   /* Width of the image */
  int height;  /* Height of the image */
  int stride;  /* Width of the memory (width + paddind such that it is a multiple of 4) */
  float *data; /* Image data, aligned */
};

/* structure for 3-channels image stored with one layer per color,
 * it assumes that data2 = data1+width*height and data3 = data2+width*height. */
struct color_image {
  int width;    /* Width of the image */
  int height;   /* Height of the image */
  int stride;   /* Width of the memory (width + paddind such that it is a multiple of 4) */
  float *data1; /* Color 1, aligned */
  float *data2; /* Color 2, consecutive to c1*/
  float *data3; /* Color 3, consecutive to c2 */
};

/* structure for convolutions */
struct convolution {
  int order;          /* Order of the convolution */
  float *coeffs;      /* Coefficients */
  float *coeffs_accu; /* Accumulated coefficients */
};

/********** Create/Delete **********/

/* allocate a new image of size width x height */
gray_image *image_new(const int width, const int height);

/* allocate a new image and copy the content from src */
gray_image *image_cpy(const gray_image *src);

/* set all pixels values to zeros */
void image_erase(gray_image *image);

/* set all pixels values to ones */
void image_ones(gray_image *image);

/* free memory of an image */
void image_delete(gray_image *image);

/* multiply an image by a scalar */
void image_mul_scalar(gray_image *image, const float scalar);

/* allocate a new color image of size width x height */
color_image *color_image_new(const int width, const int height);

/* allocate a new color image and copy the content from src */
color_image *color_image_cpy(const color_image *src);

/* set all pixels values to zeros */
void color_image_erase(color_image *image);

/* set all pixels values to ones */
void color_image_ones(color_image *image);

/* free memory of a color image */
void color_image_delete(color_image *image);

/* color image to gray image */
gray_image *color_to_gray(const color_image *src);

/************ Convolution ******/

/* return half coefficient of a gaussian filter */
float *gaussian_filter(const float sigma, int *fSize);

/* create a convolution structure with a given order, half_coeffs, symmetric or anti-symmetric
 according to even parameter */
convolution *convolution_new(int order, const float *half_coeffs, const int even);

/* perform an horizontal convolution of an image */
void convolve_horiz(gray_image *dest, const gray_image *src, const convolution *conv);

/* perform a vertical convolution of an image */
void convolve_vert(gray_image *dest, const gray_image *src, const convolution *conv);

/* free memory of a convolution structure */
void convolution_delete(convolution *conv);

/* perform horizontal and/or vertical convolution to a color image */
void color_image_convolve_hv(color_image *dst, const color_image *src, const convolution *horiz_conv,
                             const convolution *vert_conv);

#endif  // !SRC_IMAGE_HPP
