//
// Created by Jia-Baos on 2023/9/22.
//
#pragma
#ifndef SRC_IMAGE_HPP
#define SRC_IMAGE_HPP

#include <stdlib.h>
#include <malloc.h>
#include <math.h>

/********** STRUCTURES *********/

/* structure for 1-channel image */
struct GrayImage {
    int width;   /* Width of the image */
    int height;  /* Height of the image */
    int stride;  /* Width of the memory (width + paddind such that it is a multiple of 4) */
    float *data; /* Image data, aligned */
};

/* structure for 3-channels image stored with one layer per color,
 * it assumes that data2 = data1+width*height and data3 = data2+width*height. */
struct ColorImage {
    int width;    /* Width of the image */
    int height;   /* Height of the image */
    int stride;   /* Width of the memory (width + paddind such that it is a multiple of 4) */
    float *data1; /* Color 1, aligned */
    float *data2; /* Color 2, consecutive to c1*/
    float *data3; /* Color 3, consecutive to c2 */
};

/* structure for convolutions */
struct Convolution {
    int order;          /* Order of the convolution */
    float *coeffs;      /* Coefficients */
    float *coeffs_accu; /* Accumulated coefficients */
};

/********** Create/Delete **********/

/* allocate a new image of size width x height */
GrayImage *ImageNew(const int width, const int height);

/* allocate a new image and copy the content from src */
GrayImage *ImageCpy(const GrayImage *src);

/* set all pixels values to zeros */
void ImageErase(GrayImage *image);

/* set all pixels values to ones */
void ImageOnes(GrayImage *image);

/* free memory of an image */
void ImageDelete(GrayImage *image);

/* multiply an image by a scalar */
void ImageMulScalar(GrayImage *image, const float scalar);

/* allocate a new color image of size width x height */
ColorImage *ColorImageNew(const int width, const int height);

/* allocate a new color image and copy the content from src */
ColorImage *ColorImageCpy(const ColorImage *src);

/* set all pixels values to zeros */
void ColorImageErase(ColorImage *image);

/* set all pixels values to ones */
void ColorImageOnes(ColorImage *image);

/* free memory of a color image */
void ColorImageDelete(ColorImage *image);

/* color image to gray image */
GrayImage *ColorToGray(const ColorImage *src);

/************ Convolution ******/

/* return half coefficient of a gaussian filter */
float *GaussianFilter(const float sigma, int *fSize);

/* create a convolution structure with a given order, half_coeffs, symmetric or anti-symmetric
 according to even parameter */
Convolution *ConvolutionNew(int order, const float *half_coeffs, const int even);

/* perform an horizontal convolution of an image */
void ConvolveHoriz(GrayImage *dest, const GrayImage *src, const Convolution *conv);

/* perform a vertical convolution of an image */
void ConvolveVert(GrayImage *dest, const GrayImage *src, const Convolution *conv);

/* free memory of a convolution structure */
void ConvolutionDelete(Convolution *conv);

/* perform horizontal and/or vertical convolution to a color image */
void ColorImageConvolveHv(ColorImage *dst, const ColorImage *src, const Convolution *horiz_conv, const Convolution *vert_conv);

#endif // !SRC_IMAGE_HPP
