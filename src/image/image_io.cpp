//
// Created by Jia-Baos on 2024/5/22.
//

#include "image_io.hpp"

/********************* IMAGE ***********************/

// PPM

typedef struct {
  int magic;
  int width;
  int height;
  int pixmax;
} ppm_hdr_t;

static void get_magic(FILE *fp, ppm_hdr_t *ppm_hdr) {
  char str[1024];
  fgets(str, 1024, fp);
  if (str[0] == 'P' && (str[1] <= '6' || str[1] >= '1')) {
    ppm_hdr->magic = str[1] - '0';
  }
}

static int skip_comment(FILE *fp) {
  char c;
  do {
    c = (char)fgetc(fp);
  } while (c == ' ' || c == '\t' || c == '\n');
  if (c == '#') {
    do {
      c = (char)fgetc(fp);

    } while (c != 0x0A);
    return 1;
  } else {
    ungetc(c, fp);
  }
  return 0;
}

/*----------------------------------------------------------------------------*/

static void skip_comments(FILE *fp) { while (skip_comment(fp)); }

/*----------------------------------------------------------------------------*/

static int get_image_size(FILE *fp, ppm_hdr_t *ppm_hdr) {
  skip_comments(fp);
  if (fscanf(fp, "%d %d", &ppm_hdr->width, &ppm_hdr->height) != 2) {
    fprintf(stderr, "Warning: PGM --> File currupted\n");
    return 0;
  }
  return 1;
}

/*----------------------------------------------------------------------------*/

static int get_pixmax(FILE *fp, ppm_hdr_t *ppm_hdr) {
  skip_comments(fp);
  ppm_hdr->pixmax = 1;
  if (ppm_hdr->magic == 2 || ppm_hdr->magic == 3 || ppm_hdr->magic == 5 || ppm_hdr->magic == 6) {
    if (fscanf(fp, "%d", &ppm_hdr->pixmax) != 1) {
      fprintf(stderr, "Warning: PGM --> pixmax not valid\n");
      return 0;
    }
  }
  fgetc(fp);
  return 1;
}

/*----------------------------------------------------------------------------*/

static int get_ppm_hdr(FILE *fp, ppm_hdr_t *ppm_hdr) {
  get_magic(fp, ppm_hdr);
  if (!get_image_size(fp, ppm_hdr)) {
    return 0;
  }
  if (!get_pixmax(fp, ppm_hdr)) {
    return 0;
  }
  return 1;
}

static void raw_read_color(FILE *fp, color_image *image) {
  int j;
  for (j = 0; j < image->height; j++) {
    int o = j * image->stride, i;
    for (i = 0; i < image->width; i++, o++) {
      image->data1[o] = (float)fgetc(fp);
      image->data2[o] = (float)fgetc(fp);
      image->data3[o] = (float)fgetc(fp);
    }
  }
}

color_image *color_image_pnm_load(FILE *fp) {
  color_image *image = NULL;
  ppm_hdr_t ppm_hdr;
  if (!get_ppm_hdr(fp, &ppm_hdr)) {
    return NULL;
  }
  switch (ppm_hdr.magic) {
    case 1: /* PBM ASCII */
    case 2: /* PGM ASCII */
    case 3: /* PPM ASCII */
    case 4: /* PBM RAW */
    case 5: /* PGM RAW */
      fprintf(stderr, "color_image_pnm_load: only PPM raw with maxval 255 supported\n");
      break;
    case 6: /* PPM RAW */
      image = color_image_new(ppm_hdr.width, ppm_hdr.height);
      raw_read_color(fp, image);
      break;
  }
  return image;
}

// JPG

// color_image *color_image_jpeg_load(FILE *fp) {
//   struct jpeg_decompress_struct cinfo;
//   struct jpeg_error_mgr jerr;
//   JSAMPARRAY buffer;
//   int row_stride;
//   int index = 0;
//   color_image *image = NULL;
//   float *r_p, *g_p, *b_p;
//   JSAMPROW buffer_p;
//   cinfo.err = jpeg_std_error(&jerr);
//   jpeg_create_decompress(&cinfo);
//   jpeg_stdio_src(&cinfo, fp);
//   jpeg_read_header(&cinfo, TRUE);
//   cinfo.out_color_space = JCS_RGB;
//   cinfo.quantize_colors = FALSE;
//   image = color_image_new(cinfo.image_width, cinfo.image_height);
//   if (image == NULL) {
//     return NULL;
//   }
//   jpeg_start_decompress(&cinfo);
//   row_stride = cinfo.output_width * cinfo.output_components;
//   buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

// r_p = image->data1;
// g_p = image->data2;
// b_p = image->data3;

// const int incr_line = image->stride - image->width;

// while (cinfo.output_scanline < cinfo.output_height) {
//   jpeg_read_scanlines(&cinfo, buffer, 1);
//   buffer_p = buffer[0];
//   index = cinfo.output_width;
//   while (index--) {
//     *r_p++ = (float)*buffer_p++;
//     *g_p++ = (float)*buffer_p++;
//     *b_p++ = (float)*buffer_p++;
//   }
//   r_p += incr_line;
//   g_p += incr_line;
//   b_p += incr_line;
// }
// jpeg_finish_decompress(&cinfo);
// jpeg_destroy_decompress(&cinfo);
// return image;
// }

// PNG

// color_image *color_image_png_load(FILE *fp, const char *file_name) {
//   // read the header
//   png_byte header[8];
//   fread(header, 1, 8, fp);

// if (png_sig_cmp(header, 0, 8)) {
//   fprintf(stderr, "error: %s is not a PNG.\n", file_name);
//   fclose(fp);
//   return 0;
// }

// png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
// if (!png_ptr) {
//   fprintf(stderr, "error: png_create_read_struct returned 0.\n");
//   fclose(fp);
//   return 0;
// }

// // create png info struct
// png_infop info_ptr = png_create_info_struct(png_ptr);
// if (!info_ptr) {
//   fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//   png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
//   fclose(fp);
//   return 0;
// }

// // create png info struct
// png_infop end_info = png_create_info_struct(png_ptr);
// if (!end_info) {
//   fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
//   fclose(fp);
//   return 0;
// }

// // the code in this if statement gets called if libpng encounters an error
// if (setjmp(png_jmpbuf(png_ptr))) {
//   fprintf(stderr, "error from libpng\n");
//   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//   fclose(fp);
//   return 0;
// }

// // init png reading
// png_init_io(png_ptr, fp);

// // let libpng know you already read the first 8 bytes
// png_set_sig_bytes(png_ptr, 8);

// // read all the info up to the image data
// png_read_info(png_ptr, info_ptr);

// // variables to pass to get info
// int bit_depth, color_type;
// png_uint_32 temp_width, temp_height;

// // get info about png
// png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type, NULL, NULL, NULL);

// // Update the png info struct.
// png_read_update_info(png_ptr, info_ptr);

// // Row size in bytes.
// int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

// // Allocate the image_data as a big block, to be given to opengl
// png_byte *image_data;
// image_data = (png_byte *)malloc(sizeof(png_byte) * rowbytes * temp_height);
// assert(image_data != NULL);

// // row_pointers is for pointing to image_data for reading the png with libpng
// png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * temp_height);
// assert(row_pointers != NULL);

// // set the individual row_pointers to point at the correct offsets of image_data
// unsigned int i;
// for (i = 0; i < temp_height; i++) row_pointers[i] = image_data + i * rowbytes;

// // read the png into image_data through row_pointers
// png_read_image(png_ptr, row_pointers);

// // copy into color image
// color_image *image = color_image_new(temp_width, temp_height);
// if (color_type == 0) {
//   assert((unsigned)rowbytes == temp_width || !"error: not a proper gray png image");
//   for (i = 0; i < temp_height; i++) {
//     unsigned char j;
//     for (j = 0; j < temp_width; j++)
//       image->data1[i * image->stride + j] = image->data2[i * image->stride + j] =
//           image->data3[i * image->stride + j] = image_data[i * image->width + j];
//   }
// } else if (color_type == 2) {
//   assert((unsigned)rowbytes == 3 * temp_width || !"error: not a proper color png image");
//   for (i = 0; i < temp_height; i++) {
//     unsigned char j;
//     for (j = 0; j < temp_width; j++) {
//       image->data1[i * image->stride + j] = image_data[3 * (i * image->width + j) + 0];
//       image->data2[i * image->stride + j] = image_data[3 * (i * image->width + j) + 1];
//       image->data3[i * image->stride + j] = image_data[3 * (i * image->width + j) + 2];
//     }
//   }
// } else
//   assert(!"error: unknown PNG color type");

// // clean up
// png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
// free(row_pointers);
// free(image_data);
// return image;
// }

// GENERAL LOAD

/* load a color image from a file */
color_image *color_image_load(const char *fname) {
  FILE *fp;
  char magic[2];
  unsigned short *magic_short = (unsigned short *)magic;
  color_image *image = NULL;
  if ((fp = fopen(fname, "rb")) == NULL) {
    fprintf(stderr, "Error in color_image_load() - can not open file `%s' !\n", fname);
    exit(1);
  }
  fread(magic, sizeof(char), 2, fp);
  rewind(fp);
  if (magic[0] == 'P' && (magic[1] == '6' || magic[1] == '5')) { /* PPM raw */
    image = color_image_pnm_load(fp);
  } else if (magic_short[0] == 0xd8ff) {
    // image = color_image_jpeg_load(fp);
    fprintf(stderr, "Error in color_image_jpeg_load() - can not open file `%s' !\n", fname);
  } else if (magic[0] == -119 && magic[1] == 'P') {
    // image = color_image_png_load(fp, fname);
    fprintf(stderr, "Error in color_image_png_load() - can not open file `%s' !\n", fname);
  } else {
    fprintf(stderr, "Error in color_image_load(%s) - image format not supported, can only read jpg or ppm\n", fname);
    exit(1);
  }
  fclose(fp);
  return image;
}

/* write the color image to a ppn file */
void color_image_write(const char *fname, const color_image *img) {
  FILE *fp;
  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "Error in color_image_load() - can not open file `%s' !\n", fname);
    exit(1);
  }

  /*
  PBM 是位图（bitmap），仅有黑与白，没有灰
  PGM 是灰度图（grayscale）
  PPM 是通过RGB三种颜色显现的图像（pixmaps）

  P1 Bitmap ASCII
  P2 Graymap ASCII
  P3 Pixmap ASCII
  P4 Bitmap Binary
  P5 Graymap Binary
  P6 Pixmap Binary

  PPM图像格式分为两部分，分别为头部分和图像数据部分。
  头部分：由3部分组成，通过换行或空格进行分割，一般PPM的标准是空格。
  第1部分：P3或P6，指明PPM的编码格式，
  第2部分：图像的宽度和高度，通过ASCII表示，
  第3部分：最大像素值，0-255字节表示。

  图像数据部分：
  ASCII格式：按RGB的顺序排列，RGB中间用空格隔开，图片每一行用回车隔开。
  Binary格式：PPM用24bits代表每一个像素，红绿蓝分别占用8bits。

  https://segmentfault.com/a/1190000016443598?utm_source=sf-similar-article
  */

  // P6 1024 436 255

  /* comment should start with # */
  const char *comment = "# this is my new binary ppm file";

  /* write header to the file */
  fprintf(fp, "P6\n%s\n%d\n%d\n%d\n", comment, img->width, img->height, 255);
  /* write image data bytes to the file */
  for (int i = 0; i < img->height; ++i) {
    for (int j = 0; j < img->width; ++j) {
      const int index = i * img->stride + j;
      unsigned char r = (unsigned char)img->data1[index];
      unsigned char g = (unsigned char)img->data2[index];
      unsigned char b = (unsigned char)img->data3[index];

      fwrite(&r, sizeof(unsigned char), 1, fp);
      fwrite(&g, sizeof(unsigned char), 1, fp);
      fwrite(&b, sizeof(unsigned char), 1, fp);
    }
  }

  fclose(fp);
}

/* write the gray image to a ppn file */
void gray_image_write(const char *fname, const gray_image *img) {
  FILE *fp;
  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "Error in color_image_load() - can not open file `%s' !\n", fname);
    exit(1);
  }

  /*
  PBM 是位图（bitmap），仅有黑与白，没有灰
  PGM 是灰度图（grayscale）
  PPM 是通过RGB三种颜色显现的图像（pixmaps）

  P1 Bitmap ASCII
  P2 Graymap ASCII
  P3 Pixmap ASCII
  P4 Bitmap Binary
  P5 Graymap Binary
  P6 Pixmap Binary

  PPM图像格式分为两部分，分别为头部分和图像数据部分。
  头部分：由3部分组成，通过换行或空格进行分割，一般PPM的标准是空格。
  第1部分：P3或P6，指明PPM的编码格式，
  第2部分：图像的宽度和高度，通过ASCII表示，
  第3部分：最大像素值，0-255字节表示。

  图像数据部分：
  ASCII格式：按RGB的顺序排列，RGB中间用空格隔开，图片每一行用回车隔开。
  Binary格式：PPM用24bits代表每一个像素，红绿蓝分别占用8bits。

  https://segmentfault.com/a/1190000016443598?utm_source=sf-similar-article
  */

  // P6 1024 436 255

  /* comment should start with # */
  const char *comment = "# this is my new binary ppm file";

  /* write header to the file */
  fprintf(fp, "P5\n%s\n%d\n%d\n%d\n", comment, img->width, img->height, 255);
  /* write image data bytes to the file */
  for (int i = 0; i < img->height; ++i) {
    for (int j = 0; j < img->width; ++j) {
      const int index = i * img->stride + j;
      unsigned char val = (unsigned char)img->data[index];

      fwrite(&val, sizeof(unsigned char), 1, fp);
    }
  }

  fclose(fp);
}