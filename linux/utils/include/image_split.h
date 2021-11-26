#ifndef _IMAGE_SPLIT_H_
#define _IMAGE_SPLIT_H_

#include <stdint.h>


#define DEFALUT_SPLIT_ALIGN (16)


typedef struct {
	uint64_t addr;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
} split_image_t;


typedef enum {
    SPLIT_DIR_WIDTH = 0,
    SPLIT_DIR_HEIGHT,
} split_dir_e;


int split_multiple_image_horizontal_scan(split_image_t *src, split_image_t *dst, int num, split_dir_e split_dir, int align);
int split_multiple_image_vertical_scan(split_image_t *src, split_image_t *dst, int num, split_dir_e split_dir, int align);


#endif


