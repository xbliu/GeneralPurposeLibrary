#ifndef _IMAGE_FILE_H_
#define _IMAGE_FILE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


int image_file_read_one_frame(FILE *fp, char *buffer,int width, int height, int stride);
int image_file_write_one_frame(FILE *fp, char *buffer,int width, int height, int stride);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif

