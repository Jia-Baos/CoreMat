#include "../src/image/image.hpp"
#include "../src/image/image_io.hpp"
#include "gtest/gtest.h"
#include <cstring>

TEST(test_image, test1)
{
    std::string img_path = "/home/seer/Project-CPP/CoreMat/images/frame_0029.ppm";
    std::string save_rgb_path = "/home/seer/Project-CPP/CoreMat/img_rgb.ppm";
    std::string save_gray_path = "/home/seer/Project-CPP/CoreMat/img_gray.ppm";

    // printf("read img...\n");
    ColorImage *img_rgb = ColorImageLoad(img_path.c_str());

    // printf("rgb to gray...\n");
    GrayImage *img_gray = ColorToGray(img_rgb);

    EXPECT_EQ(img_gray->height, img_rgb->height);
    EXPECT_EQ(img_gray->width, img_rgb->width);
    EXPECT_EQ(img_gray->stride, img_rgb->stride);

    // printf("smooth the gray img...\n");
    GrayImage *img_gray_cpy = ImageCpy(img_gray);
    float deriv_filter1[3] = { 0.0f, -8.0f / 12.0f, 1.0f / 12.0f };
    Convolution *filter1 = ConvolutionNew(2, deriv_filter1, 0);

    EXPECT_EQ(filter1->order, 2);

    ConvolveHoriz(img_gray_cpy, img_gray, filter1);
    ConvolveVert(img_gray_cpy, img_gray, filter1);
    ConvolutionDelete(filter1);

    // printf("smooth the rgb img...\n");
    ColorImage *img_rgb_cpy = ColorImageCpy(img_rgb);
    float deriv_filter2[2] = { 0.0f, -0.5f };
    Convolution *filter2 = ConvolutionNew(1, deriv_filter2, 0);

    EXPECT_EQ(filter2->order, 1);

    ColorImageConvolveHv(img_rgb_cpy, img_rgb, filter2, filter2);
    ConvolutionDelete(filter2);

    EXPECT_EQ(img_gray_cpy->height, img_rgb_cpy->height);
    EXPECT_EQ(img_gray_cpy->width, img_rgb_cpy->width);
    EXPECT_EQ(img_gray_cpy->stride, img_rgb_cpy->stride);

    // printf("delete img...\n");
    ImageDelete(img_gray);
    ImageDelete(img_gray_cpy);
    ColorImageDelete(img_rgb);
    ColorImageDelete(img_rgb_cpy);
}

TEST(test_image, test2)
{
    float deriv_filter1[3] = { 0.0f, -8.0f / 12.0f, 1.0f / 12.0f };
    Convolution *filter1 = ConvolutionNew(2, deriv_filter1, 0);
    EXPECT_EQ(filter1->order, 2);
    ConvolutionDelete(filter1);

    float deriv_filter2[2] = { 0.0f, -0.5f };
    Convolution *filter2 = ConvolutionNew(1, deriv_filter2, 0);
    EXPECT_EQ(filter2->order, 1);
    ConvolutionDelete(filter2);
}
