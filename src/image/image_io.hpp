//
// Created by Jia-Baos on 2024/5/22.
//

#pragma
#ifndef SRC_IMAGE_IO_HPP
#define SRC_IMAGE_IO_HPP

#include <stdlib.h>
#include "./image.hpp"

/* load a color image from a file in jpg or ppm*/
ColorImage *ColorImageLoad(const char *fname);

/* write the color image to a ppn file */
void ColorImageWrite(const char *fname, const ColorImage *img);

/* write the gray image to a ppn file */
void GrayImageWrite(const char *fname, const GrayImage *img);

#endif // !SRC_IMAGE_IO_HPP
