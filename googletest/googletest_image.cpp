#include "../src/image/image.hpp"
#include "../src/image/image_io.hpp"
#include "gtest/gtest.h"

TEST(test_image, test1) {
  std::string img_path = "/home/jia-baos/Project-CPP/CoreMat/images/frame_0029.ppm";
  std::string save_rgb_path = "/home/jia-baos/Project-CPP/CoreMat/img_rgb.ppm";
  std::string save_gray_path = "/home/jia-baos/Project-CPP/CoreMat/img_gray.ppm";

  // printf("read img...\n");
  color_image* img_rgb = color_image_load(img_path.c_str());

  // printf("rgb to gray...\n");
  gray_image* img_gray = color_to_gray(img_rgb);

  EXPECT_EQ(img_gray->height, img_rgb->height);
  EXPECT_EQ(img_gray->width, img_rgb->width);
  EXPECT_EQ(img_gray->stride, img_rgb->stride);

  // printf("smooth the gray img...\n");
  gray_image* img_gray_cpy = image_cpy(img_gray);
  float deriv_filter1[3] = {0.0f, -8.0f / 12.0f, 1.0f / 12.0f};
  convolution* filter1 = convolution_new(2, deriv_filter1, 0);

  EXPECT_EQ(filter1->order, 2);

  convolve_horiz(img_gray_cpy, img_gray, filter1);
  convolve_vert(img_gray_cpy, img_gray, filter1);
  convolution_delete(filter1);

  // printf("smooth the rgb img...\n");
  color_image* img_rgb_cpy = color_image_cpy(img_rgb);
  float deriv_filter2[2] = {0.0f, -0.5f};
  convolution* filter2 = convolution_new(1, deriv_filter2, 0);

  EXPECT_EQ(filter2->order, 1);

  color_image_convolve_hv(img_rgb_cpy, img_rgb, filter2, filter2);
  convolution_delete(filter2);

  EXPECT_EQ(img_gray_cpy->height, img_rgb_cpy->height);
  EXPECT_EQ(img_gray_cpy->width, img_rgb_cpy->width);
  EXPECT_EQ(img_gray_cpy->stride, img_rgb_cpy->stride);

  // printf("delete img...\n");
  image_delete(img_gray);
  image_delete(img_gray_cpy);
  color_image_delete(img_rgb);
  color_image_delete(img_rgb_cpy);
}

TEST(test_image, test2) {
  float deriv_filter1[3] = {0.0f, -8.0f / 12.0f, 1.0f / 12.0f};
  convolution* filter1 = convolution_new(2, deriv_filter1, 0);
  EXPECT_EQ(filter1->order, 2);
  convolution_delete(filter1);

  float deriv_filter2[2] = {0.0f, -0.5f};
  convolution* filter2 = convolution_new(1, deriv_filter2, 0);
  EXPECT_EQ(filter2->order, 1);
  convolution_delete(filter2);
}