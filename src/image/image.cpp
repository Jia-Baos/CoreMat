//
// Created by Jia-Baos on 2023/9/22.
//

#include "image.hpp"

#include <iostream>
#include <cmath>
#include <cstring>
#include <xmmintrin.h>

using v4sf = __v4sf;

/********** Create/Delete **********/

/* allocate a new image of size width x height */
GrayImage *ImageNew(const int width, const int height)
{
    GrayImage *image = (GrayImage *)malloc(sizeof(GrayImage));
    if (image == nullptr) {
        std::cerr << "Error: image_new() - not enough memory !" << std::endl;
        exit(1);
    }
    image->width = width;
    image->height = height;
    image->stride = ((width + 4 - 1) / 4) * 4;
    image->data = (float *)aligned_alloc(16, image->stride * height * sizeof(float));
    if (image->data == nullptr) {
        std::cerr << "Error: image_new() - not enough memory !" << std::endl;
        exit(1);
    }
    return image;
}

/* allocate a new image and copy the content from src */
GrayImage *ImageCpy(const GrayImage *src)
{
    GrayImage *dst = ImageNew(src->width, src->height);
    memcpy(dst->data, src->data, src->stride * src->height * sizeof(float));
    return dst;
}

/* set all pixels values to zeros */
void ImageErase(GrayImage *image)
{
    memset(image->data, 0, image->stride * image->height * sizeof(float));
}

/* set all pixels values to ones */
void ImageOnes(GrayImage *image)
{
    int i;
    v4sf *imp = (v4sf *)image->data;
    const v4sf kScalarp = { 1.0f, 1.0f, 1.0f, 1.0f };
    for (i = 0; i < (image->stride / 4) * image->height; i++) {
        *imp = kScalarp;
        imp += 1;
    }
}

/* multiply an image by a scalar */
void ImageMulScalar(GrayImage *image, const float scalar)
{
    int i;
    v4sf *imp = (v4sf *)image->data;
    const v4sf kScalarp = { scalar, scalar, scalar, scalar };
    for (i = 0; i < (image->stride / 4) * image->height; i++) {
        (*imp) *= kScalarp;
        imp += 1;
    }
}

/* free memory of an image */
void ImageDelete(GrayImage *image)
{
    if (image == nullptr) {
        std::cerr << "Warning: Delete image --> Ignore action (image not allocated)" << std::endl;
    } else {
        free(image->data);
        free(image);
    }
}

/* allocate a new color image of size width x height */
ColorImage *ColorImageNew(const int width, const int height)
{
    ColorImage *image = (ColorImage *)malloc(sizeof(ColorImage));
    if (image == nullptr) {
        std::cerr << "Error: color_image_new() - not enough memory !" << std::endl;
        exit(1);
    }
    image->width = width;
    image->height = height;
    image->stride = ((width + 4 - 1) / 4) * 4;
    image->data1 = (float *)aligned_alloc(16, 3 * image->stride * height * sizeof(float));
    if (image->data1 == nullptr) {
        std::cerr << "Error: color_image_new() - not enough memory !" << std::endl;
        exit(1);
    }
    image->data2 = image->data1 + image->stride * height;
    image->data3 = image->data2 + image->stride * height;
    return image;
}

/* allocate a new color image and copy the content from src */
ColorImage *ColorImageCpy(const ColorImage *src)
{
    ColorImage *dst = ColorImageNew(src->width, src->height);
    memcpy(dst->data1, src->data1, 3 * src->stride * src->height * sizeof(float));
    return dst;
}

/* set all pixels values to zeros */
void ColorImageErase(ColorImage *image)
{
    memset(image->data1, 0, 3 * image->stride * image->height * sizeof(float));
}

/* set all pixels values to ones */
void ColorImageOnes(ColorImage *image)
{
    int i;
    v4sf *imp = (v4sf *)image->data1;
    const v4sf kScalarp = { 1.0f, 1.0f, 1.0f, 1.0f };
    for (i = 0; i < 3 * (image->stride / 4) * image->height; i++) {
        *imp = kScalarp;
        imp += 1;
    }
}

/* free memory of a color image */
void ColorImageDelete(ColorImage *image)
{
    if (image == nullptr) {
        std::cerr << "Warning: Delete image --> Ignore action (image not allocated)" << std::endl;
    } else {
        free(image->data1); // c2 and c3 was allocated at the same moment
        free(image);
    }
}

/* color image to gray image */
GrayImage *ColorToGray(const ColorImage *src)
{
    GrayImage *dst = ImageNew(src->width, src->height);

    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            const int kIndex = i * src->stride + j;
            dst->data[kIndex] = 0.299 * src->data1[kIndex] + 0.587 * src->data2[kIndex] + 0.114 * src->data3[kIndex];
        }
    }

    return dst;
}

/************ Convolution ******/

/* return half coefficient of a gaussian filter
Details:
- return a float* containing the coefficient from middle to border of the filter, so starting by 0,
- it so contains half of the coefficient.
- sigma is the standard deviation.
- filter_order is an output where the size of the output array is stored */
float *GaussianFilter(const float sigma, int *filter_order)
{
    if (sigma == 0.0f) {
        std::cerr << "gaussian_filter() error: sigma is zeros" << std::endl;
        exit(1);
    }
    if (!filter_order) {
        std::cerr << "gaussian_filter() error: filter_order is null" << std::endl;
        exit(1);
    }
    // computer the filter order as 1 + 2* floor(3*sigma)
    *filter_order = floor(3 * sigma) + 1;
    if (*filter_order == 0) {
        *filter_order = 1;
    }

    // compute coefficients
    float *data = (float *)malloc(sizeof(float) * (2 * (*filter_order) + 1));
    if (data == nullptr) {
        std::cerr << "gaussian_filter() error: not enough memory" << std::endl;
        exit(1);
    }
    const float kAlpha = 1.0f / (2.0f * sigma * sigma);
    float sum = 0.0f;
    int i;
    for (i = -(*filter_order); i <= *filter_order; i++) {
        data[i + (*filter_order)] = exp(-i * i * kAlpha);
        sum += data[i + (*filter_order)];
    }
    for (i = -(*filter_order); i <= *filter_order; i++) {
        data[i + (*filter_order)] /= sum;
    }
    // fill the output
    float *data2 = (float *)malloc(sizeof(float) * (*filter_order + 1));
    if (data2 == nullptr) {
        std::cerr << "gaussian_filter() error: not enough memory" << std::endl;
        exit(1);
    }
    memcpy(data2, &data[*filter_order], sizeof(float) * (*filter_order) + sizeof(float));
    free(data);
    return data2;
}

/* given half of the coef, compute the full coefficients and the accumulated coefficients
 * even is 0, get the deriv
 * even is 1, get the filter
 */
static void ConvolveExtractCoeffs(const int order, const float *half_coeffs, float *coeffs, float *coeffs_accu, const int even)
{
    int i;
    float accu = 0.0;
    if (even) {
        for (i = 0; i <= order; i++) {
            coeffs[order - i] = coeffs[order + i] = half_coeffs[i];
        }
        for (i = 0; i <= order; i++) {
            accu += coeffs[i];
            coeffs_accu[2 * order - i] = coeffs_accu[i] = accu;
        }
    } else {
        for (i = 0; i <= order; i++) {
            coeffs[order - i] = +half_coeffs[i];
            coeffs[order + i] = -half_coeffs[i];
        }
        for (i = 0; i <= order; i++) {
            accu += coeffs[i];
            coeffs_accu[i] = accu;
            coeffs_accu[2 * order - i] = -accu;
        }
    }
}

/* create a convolution structure with a given order, half_coeffs, symmetric or anti-symmetric according to even
 * parameter */
Convolution *ConvolutionNew(const int order, const float *half_coeffs, const int even)
{
    Convolution *conv = (Convolution *)malloc(sizeof(Convolution));
    if (conv == nullptr) {
        std::cerr << "Error: convolution_new() - not enough memory !" << std::endl;
        exit(1);
    }
    conv->order = order;
    conv->coeffs = (float *)malloc((2 * order + 1) * sizeof(float));
    if (conv->coeffs == nullptr) {
        std::cerr << "Error: convolution_new() - not enough memory !" << std::endl;
        free(conv);
        exit(1);
    }
    conv->coeffs_accu = (float *)malloc((2 * order + 1) * sizeof(float));
    if (conv->coeffs_accu == nullptr) {
        std::cerr << "Error: convolution_new() - not enough memory !" << std::endl;
        free(conv->coeffs);
        free(conv);
        exit(1);
    }
    ConvolveExtractCoeffs(order, half_coeffs, conv->coeffs, conv->coeffs_accu, even);
    return conv;
}

static void ConvolveVertFast3(GrayImage *dst, const GrayImage *src, const Convolution *conv)
{
    const int kIterline = (src->stride >> 2) + 1;
    const float *coeff = conv->coeffs;
    // const float *coeff_accu = conv->coeffs_accu;
    v4sf *srcp = (v4sf *)src->data;
    v4sf *dstp = (v4sf *)dst->data;
    v4sf *srcp_p1 = (v4sf *)(src->data + src->stride);
    int i;
    for (i = kIterline; --i;) { // first line
        *dstp = (coeff[0] + coeff[1]) * (*srcp) + coeff[2] * (*srcp_p1);
        dstp += 1;
        srcp += 1;
        srcp_p1 += 1;
    }
    v4sf *srcp_m1 = (v4sf *)src->data;
    for (i = src->height - 1; --i;) { // others line
        int j;
        for (j = kIterline; --j;) {
            *dstp = coeff[0] * (*srcp_m1) + coeff[1] * (*srcp) + coeff[2] * (*srcp_p1);
            dstp += 1;
            srcp_m1 += 1;
            srcp += 1;
            srcp_p1 += 1;
        }
    }
    for (i = kIterline; --i;) { // last line
        *dstp = coeff[0] * (*srcp_m1) + (coeff[1] + coeff[2]) * (*srcp);
        dstp += 1;
        srcp_m1 += 1;
        srcp += 1;
    }
}

static void ConvolveVertFast5(GrayImage *dst, const GrayImage *src, const Convolution *conv)
{
    const int kIterline = (src->stride >> 2) + 1;
    const float *coeff = conv->coeffs;
    // const float *coeff_accu = conv->coeffs_accu;
    v4sf *srcp = (v4sf *)src->data;
    v4sf *dstp = (v4sf *)dst->data;
    v4sf *srcp_p1 = (v4sf *)(src->data + src->stride);
    v4sf *srcp_p2 = (v4sf *)(src->data + 2 * src->stride);
    int i;
    for (i = kIterline; --i;) { // first line
        *dstp = (coeff[0] + coeff[1] + coeff[2]) * (*srcp) + coeff[3] * (*srcp_p1) + coeff[4] * (*srcp_p2);
        dstp += 1;
        srcp += 1;
        srcp_p1 += 1;
        srcp_p2 += 1;
    }
    v4sf *srcp_m1 = (v4sf *)src->data;
    for (i = kIterline; --i;) { // second line
        *dstp = (coeff[0] + coeff[1]) * (*srcp_m1) + coeff[2] * (*srcp) + coeff[3] * (*srcp_p1) + coeff[4] * (*srcp_p2);
        dstp += 1;
        srcp_m1 += 1;
        srcp += 1;
        srcp_p1 += 1;
        srcp_p2 += 1;
    }
    v4sf *srcp_m2 = (v4sf *)src->data;
    for (i = src->height - 3; --i;) { // others line
        int j;
        for (j = kIterline; --j;) {
            *dstp = coeff[0] * (*srcp_m2) + coeff[1] * (*srcp_m1) + coeff[2] * (*srcp) + coeff[3] * (*srcp_p1) + coeff[4] * (*srcp_p2);
            dstp += 1;
            srcp_m2 += 1;
            srcp_m1 += 1;
            srcp += 1;
            srcp_p1 += 1;
            srcp_p2 += 1;
        }
    }
    for (i = kIterline; --i;) { // second to last line
        *dstp = coeff[0] * (*srcp_m2) + coeff[1] * (*srcp_m1) + coeff[2] * (*srcp) + (coeff[3] + coeff[4]) * (*srcp_p1);
        dstp += 1;
        srcp_m2 += 1;
        srcp_m1 += 1;
        srcp += 1;
        srcp_p1 += 1;
    }
    for (i = kIterline; --i;) { // last line
        *dstp = coeff[0] * (*srcp_m2) + coeff[1] * (*srcp_m1) + (coeff[2] + coeff[3] + coeff[4]) * (*srcp);
        dstp += 1;
        srcp_m2 += 1;
        srcp_m1 += 1;
        srcp += 1;
    }
}

static void ConvolveHorizFast3(GrayImage *dst, const GrayImage *src, const Convolution *conv)
{
    const int kStrideMinus1 = src->stride - 1;
    const int kIterline = (src->stride >> 2);
    const float *coeff = conv->coeffs;
    v4sf *srcp = (v4sf *)src->data, *dstp = (v4sf *)dst->data;
    // create shifted version of src
    float *src_p1 = (float *)malloc(sizeof(float) * src->stride);
    float *src_m1 = (float *)malloc(sizeof(float) * src->stride);
    int j;
    for (j = 0; j < src->height; j++) {
        int i;
        float *srcptr = (float *)srcp;
        const float kRightCoef = srcptr[src->width - 1];
        for (i = src->width; i < src->stride; i++) {
            srcptr[i] = kRightCoef;
        }

        src_m1[0] = srcptr[0];
        memcpy(src_m1 + 1, srcptr, sizeof(float) * kStrideMinus1);
        src_p1[kStrideMinus1] = kRightCoef;
        memcpy(src_p1, srcptr + 1, sizeof(float) * kStrideMinus1);
        v4sf *srcp_p1 = (v4sf *)src_p1, *srcp_m1 = (v4sf *)src_m1;

        for (i = 0; i < kIterline; i++) {
            *dstp = coeff[0] * (*srcp_m1) + coeff[1] * (*srcp) + coeff[2] * (*srcp_p1);
            dstp += 1;
            srcp_m1 += 1;
            srcp += 1;
            srcp_p1 += 1;
        }
    }
    free(src_p1);
    free(src_m1);
}

static void ConvolveHorizFast5(GrayImage *dst, const GrayImage *src, const Convolution *conv)
{
    const int kStrideMinus1 = src->stride - 1;
    const int kStrideMinus2 = src->stride - 2;
    const int kIterline = (src->stride >> 2);
    const float *coeff = conv->coeffs;
    v4sf *srcp = (v4sf *)src->data, *dstp = (v4sf *)dst->data;
    float *src_p1 = (float *)malloc(sizeof(float) * src->stride * 4);
    float *src_p2 = src_p1 + src->stride;
    float *src_m1 = src_p2 + src->stride;
    float *src_m2 = src_m1 + src->stride;
    int j;
    for (j = 0; j < src->height; j++) {
        int i;
        float *srcptr = (float *)srcp;
        const float kRightCoef = srcptr[src->width - 1];
        for (i = src->width; i < src->stride; i++) {
            srcptr[i] = kRightCoef;
        }

        src_m1[0] = srcptr[0];
        memcpy(src_m1 + 1, srcptr, sizeof(float) * kStrideMinus1);
        src_m2[0] = srcptr[0];
        src_m2[1] = srcptr[0];
        memcpy(src_m2 + 2, srcptr, sizeof(float) * kStrideMinus2);
        src_p1[kStrideMinus1] = kRightCoef;
        memcpy(src_p1, srcptr + 1, sizeof(float) * kStrideMinus1);
        src_p2[kStrideMinus1] = kRightCoef;
        src_p2[kStrideMinus2] = kRightCoef;
        memcpy(src_p2, srcptr + 2, sizeof(float) * kStrideMinus2);

        v4sf *srcp_p1 = (v4sf *)src_p1;
        v4sf *srcp_p2 = (v4sf *)src_p2;
        v4sf *srcp_m1 = (v4sf *)src_m1;
        v4sf *srcp_m2 = (v4sf *)src_m2;

        for (i = 0; i < kIterline; i++) {
            *dstp = coeff[0] * (*srcp_m2) + coeff[1] * (*srcp_m1) + coeff[2] * (*srcp) + coeff[3] * (*srcp_p1) + coeff[4] * (*srcp_p2);
            dstp += 1;
            srcp_m2 += 1;
            srcp_m1 += 1;
            srcp += 1;
            srcp_p1 += 1;
            srcp_p2 += 1;
        }
    }
    free(src_p1);
}

/* perform an horizontal convolution of an image */
void ConvolveHoriz(GrayImage *dest, const GrayImage *src, const Convolution *conv)
{
    if (conv->order == 1) {
        ConvolveHorizFast3(dest, src, conv);
        return;
    } else if (conv->order == 2) {
        ConvolveHorizFast5(dest, src, conv);
        return;
    }
    float *in = src->data;
    float *out = dest->data;
    int i, j, ii;
    float *o = out;
    int i0 = -conv->order;
    int i1 = +conv->order;
    float *coeff = conv->coeffs + conv->order;
    float *coeff_accu = conv->coeffs_accu + conv->order;
    for (j = 0; j < src->height; j++) {
        const float *al = in + j * src->stride;
        const float *f0 = coeff + i0;
        float sum;
        for (i = 0; i < -i0; i++) {
            sum = coeff_accu[-i - 1] * al[0];
            for (ii = i1 + i; ii >= 0; ii--) {
                sum += coeff[ii - i] * al[ii];
            }
            *o++ = sum;
        }
        for (; i < src->width - i1; i++) {
            sum = 0;
            for (ii = i1 - i0; ii >= 0; ii--) {
                sum += f0[ii] * al[ii];
            }
            al++;
            *o++ = sum;
        }
        for (; i < src->width; i++) {
            sum = coeff_accu[src->width - i] * al[src->width - i0 - 1 - i];
            for (ii = src->width - i0 - 1 - i; ii >= 0; ii--) {
                sum += f0[ii] * al[ii];
            }
            al++;
            *o++ = sum;
        }
        for (i = 0; i < src->stride - src->width; i++) {
            o++;
        }
    }
}

/* perform a vertical convolution of an image */
void ConvolveVert(GrayImage *dest, const GrayImage *src, const Convolution *conv)
{
    if (conv->order == 1) {
        ConvolveVertFast3(dest, src, conv);
        return;
    } else if (conv->order == 2) {
        ConvolveVertFast5(dest, src, conv);
        return;
    }
    float *in = src->data;
    float *out = dest->data;
    int i0 = -conv->order;
    int i1 = +conv->order;
    float *coeff = conv->coeffs + conv->order;
    float *coeff_accu = conv->coeffs_accu + conv->order;
    int i, j, ii;
    float *o = out;
    const float *alast = in + src->stride * (src->height - 1);
    const float *f0 = coeff + i0;
    for (i = 0; i < -i0; i++) {
        float fa = coeff_accu[-i - 1];
        const float *al = in + i * src->stride;
        for (j = 0; j < src->width; j++) {
            float sum = fa * in[j];
            for (ii = -i; ii <= i1; ii++) {
                sum += coeff[ii] * al[j + ii * src->stride];
            }
            *o++ = sum;
        }
        for (j = 0; j < src->stride - src->width; j++) {
            o++;
        }
    }
    for (; i < src->height - i1; i++) {
        const float *al = in + (i + i0) * src->stride;
        for (j = 0; j < src->width; j++) {
            float sum = 0;
            const float *al2 = al;
            for (ii = 0; ii <= i1 - i0; ii++) {
                sum += f0[ii] * al2[0];
                al2 += src->stride;
            }
            *o++ = sum;
            al++;
        }
        for (j = 0; j < src->stride - src->width; j++) {
            o++;
        }
    }
    for (; i < src->height; i++) {
        float fa = coeff_accu[src->height - i];
        const float *al = in + i * src->stride;
        for (j = 0; j < src->width; j++) {
            float sum = fa * alast[j];
            for (ii = i0; ii <= src->height - 1 - i; ii++) {
                sum += coeff[ii] * al[j + ii * src->stride];
            }
            *o++ = sum;
        }
        for (j = 0; j < src->stride - src->width; j++) {
            o++;
        }
    }
}

/* free memory of a convolution structure */
void ConvolutionDelete(Convolution *conv)
{
    if (conv) {
        free(conv->coeffs);
        free(conv->coeffs_accu);
        free(conv);
    }
}

/* perform horizontal and/or vertical convolution to a color image */
void ColorImageConvolveHv(ColorImage *dst, const ColorImage *src, const Convolution *horiz_conv, const Convolution *vert_conv)
{
    const int kWidth = src->width;
    const int kHeight = src->height;
    const int kStride = src->stride;
    // separate channels of images
    GrayImage src_red = { kWidth, kHeight, kStride, src->data1 };
    GrayImage src_green = { kWidth, kHeight, kStride, src->data2 };
    GrayImage src_blue = { kWidth, kHeight, kStride, src->data3 };
    GrayImage dst_red = { kWidth, kHeight, kStride, dst->data1 };
    GrayImage dst_green = { kWidth, kHeight, kStride, dst->data2 };
    GrayImage dst_blue = { kWidth, kHeight, kStride, dst->data3 };
    // horizontal and vertical
    if (horiz_conv != nullptr && vert_conv != nullptr) {
        float *tmp_data = (float *)malloc(sizeof(float) * kStride * kHeight);
        if (tmp_data == nullptr) {
            std::cerr << "error color_image_convolve_hv(): not enough memory" << std::endl;
            exit(1);
        }
        GrayImage tmp = { kWidth, kHeight, kStride, tmp_data };
        // perform convolution for each channel
        ConvolveHoriz(&tmp, &src_red, horiz_conv);
        ConvolveVert(&dst_red, &tmp, vert_conv);
        ConvolveHoriz(&tmp, &src_green, horiz_conv);
        ConvolveVert(&dst_green, &tmp, vert_conv);
        ConvolveHoriz(&tmp, &src_blue, horiz_conv);
        ConvolveVert(&dst_blue, &tmp, vert_conv);
        free(tmp_data);
    } else if (horiz_conv != NULL && vert_conv == NULL) { // only horizontal
        ConvolveHoriz(&dst_red, &src_red, horiz_conv);
        ConvolveHoriz(&dst_green, &src_green, horiz_conv);
        ConvolveHoriz(&dst_blue, &src_blue, horiz_conv);
    } else if (vert_conv != NULL && horiz_conv == NULL) { // only vertical
        ConvolveVert(&dst_red, &src_red, vert_conv);
        ConvolveVert(&dst_green, &src_green, vert_conv);
        ConvolveVert(&dst_blue, &src_blue, vert_conv);
    }
}
