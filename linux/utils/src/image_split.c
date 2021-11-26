#include "image_split.h"
#include "log_adapter.h"


int split_multiple_image_horizontal_scan(split_image_t *src, split_image_t *dst, int num, split_dir_e split_dir, int align)
{
    int i = 0;
	uint32_t width = 0;
	uint32_t height = 0;

	if (!src || !dst) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    if (SPLIT_DIR_WIDTH == split_dir) {
        width = ((src->width / num) / align) * align;
		for (i=0; i< num; i++) {
			dst[i].addr = src->addr + i * width;
			dst[i].width = width;
			dst[i].height = src->height;
			dst[i].stride = src->stride;
		}
    } else {
        height = ((src->height / num) / align) * align;
		for (i=0; i< num; i++) {
			dst[i].addr = src->addr + i * src->stride * height;
			dst[i].width = src->width;
			dst[i].height = height;
			dst[i].stride = src->stride;
		}
    }

    return 0;
}

int split_multiple_image_vertical_scan(split_image_t *src, split_image_t *dst, int num, split_dir_e split_dir, int align)
{
    int i = 0;
	uint32_t width = 0;
	uint32_t height = 0;

	if (!src || !dst) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    if (SPLIT_DIR_WIDTH == split_dir) {
        width = ((src->width / num) / align) * align;
		for (i=0; i< num; i++) {
			dst[i].addr = src->addr + i * width * src->stride;
			dst[i].width = width;
			dst[i].height = src->height;
			dst[i].stride = src->stride;
		}
    } else {
        height = ((src->height / num) / align) * align;
		for (i=0; i< num; i++) {
			dst[i].addr = src->addr + i * src->width * height;
			dst[i].width = src->width;
			dst[i].height = height;
			dst[i].stride = src->stride;
		}
    }

    return 0;
}

