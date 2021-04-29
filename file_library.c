#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>


int64_t gplib_get_file_size(const char *name)
{
	int64_t size = 0;

	int fd = open(name,O_RDONLY);
	if (fd < 0) {
		fprintf(stderr,"can't open %s file\n",name);
		return -1;
	}

	size = lseek(fd,0,SEEK_END);
	if (size < 0) {
		fprintf(stderr,"failed to seek %s file\n",name);
	}
	
	close(fd);

	return size;
}


