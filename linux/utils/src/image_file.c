#include "log_adapter.h"

#include <stdio.h>
#include <errno.h>


typedef size_t (*fops_func)(void *ptr, size_t size, size_t nmemb, FILE *stream);


static int image_file_rw_one_frame(FILE *fp, char *buffer,int width, int height, int stride, fops_func rw_ops)
{
	int row = 0;
    int ret = 0;
	char *ptr = NULL;

	ptr = buffer;
    for (row = 0; row < height; row++) {
        ret = rw_ops(ptr, width, 1, fp);
		if (1 == ret) {
			ptr += stride;
			continue;
		}
		
		if (feof(fp)) {
			LOG_INFO(LOG_MOD_UTILS, "reach end of file!\n");
		} else {
			LOG_ERROR(LOG_MOD_UTILS, "image read or write file failed!\n");
		}
		break;
    }

    return 0;
}


int image_file_read_one_frame(FILE *fp, char *buffer,int width, int height, int stride)
{
	return image_file_rw_one_frame(fp,buffer,width,height,stride,(fops_func)fread);
}

int image_file_write_one_frame(FILE *fp, char *buffer,int width, int height, int stride)
{
    return image_file_rw_one_frame(fp,buffer,width,height,stride,(fops_func)fwrite);
}

