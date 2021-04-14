#ifndef __GIMP_IMAGE_H
#define __GIMP_IMAGE_H

#include <pgmspace.h>
#include <stdint.h>

typedef uint16_t guint;
typedef uint8_t guint8;

typedef struct {
  guint  	 width;
  guint  	 height;
  guint  	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  guint8 	 pixel_data[320 * 320 * 3 + 1];
} gimp_image_t;

#endif//__GIMP_IMAGE_H
