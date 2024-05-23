#include <iostream>
#include <string>
#include "../image/image.hpp"
#include "../image/image_io.hpp"

int main() {
  printf("Test image func...\n");
  std::string img_path = "D:/CmakeProject/CoreMat/images/frame_0029.ppm";
  std::string save_rgb_path = "D:/CmakeProject/CoreMat/img_rgb.ppm";
  std::string save_gray_path = "D:/CmakeProject/CoreMat/img_gray.ppm";

  printf("read img...\n");
  color_image* img_rgb = color_image_load(img_path.c_str());

  printf("rgb to gray...\n");
  gray_image* img_gray = color_to_gray(img_rgb);

  printf("smooth the gray img...\n");
  gray_image* img_gray_cpy = image_cpy(img_gray);
  float deriv_filter1[3] = {0.0f, -8.0f / 12.0f, 1.0f / 12.0f};
  convolution* filter1 = convolution_new(2, deriv_filter1, 0);
  convolve_horiz(img_gray_cpy, img_gray, filter1);
  convolve_vert(img_gray_cpy, img_gray, filter1);
  convolution_delete(filter1);

  printf("smooth the rgb img...\n");
  color_image* img_rgb_cpy = color_image_cpy(img_rgb);
  float deriv_filter2[2] = {0.0f, -0.5f};
  convolution* filter2 = convolution_new(1, deriv_filter2, 0);
  color_image_convolve_hv(img_rgb_cpy, img_rgb, filter2, filter2);
  convolution_delete(filter2);

  printf("save img to file...\n");
  color_image_write(save_rgb_path.c_str(), img_rgb_cpy);
  gray_image_write(save_gray_path.c_str(), img_gray_cpy);

  printf("delete img...\n");
  image_delete(img_gray);
  image_delete(img_gray_cpy);
  color_image_delete(img_rgb);
  color_image_delete(img_rgb_cpy);

  return 0;
}
