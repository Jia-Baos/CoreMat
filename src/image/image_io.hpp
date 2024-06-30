//
// Created by Jia-Baos on 2024/5/22.
//

#pragma
#ifndef SRC_IMAGE_IO_HPP
#define SRC_IMAGE_IO_HPP

#include <stdlib.h>
#include "./image.hpp"

/* load a color image from a file in jpg or ppm*/
color_image *color_image_load(const char *fname);

/* write the color image to a ppn file */
void color_image_write(const char *fname, const color_image *img);

/* write the gray image to a ppn file */
void gray_image_write(const char *fname, const gray_image *img);

#endif  // !SRC_IMAGE_IO_HPP