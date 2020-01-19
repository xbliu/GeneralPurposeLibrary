#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>


int fs_mkdirs(const char* path, mode_t mode)
{
    int res = 0;
    int fd = 0;
    struct stat sb;
    char* buf = strdup(path);
    char* segment = buf + 1;
    char* p = segment;

    if (*buf != '/') {
        fprintf(stderr,"Relative paths are not allowed: %s \n", buf);
        res = -EINVAL;
        goto done;
    }

    if ((fd = open("/", 0)) == -1) {
        fprintf(stderr,"Failed to open(/): %s \n", strerror(errno));
        res = -errno;
        goto done;
    }
    
    while (*p != '\0') {
		if (*p != '/') {
			p++;
			continue;
		}
		
		*p = '\0';

		if (!strcmp(segment, "..") || !strcmp(segment, ".") || !strcmp(segment, "")) {
			fprintf(stderr,"Invalid path: %s \n", buf);
			res = -EINVAL;
			goto done_close;
		}

		if (fstatat(fd, segment, &sb, AT_SYMLINK_NOFOLLOW) != 0) {
			if (errno == ENOENT) {
				/* Nothing there yet; let's create it! */
				if (mkdirat(fd, segment, mode) != 0) {
					if (errno == EEXIST) {
						/* We raced with someone; ignore */
					} else {
						fprintf(stderr,"Failed to mkdirat(%s): %s \n", buf, strerror(errno));
						res = -errno;
						goto done_close;
					}
				}
			} else {
				fprintf(stderr,"Failed to fstatat(%s): %s \n", buf, strerror(errno));
				res = -errno;
				goto done_close;
			}
		} else {
			if (S_ISLNK(sb.st_mode)) {
				fprintf(stderr,"Symbolic links are not allowed: %s \n", buf);
				res = -ELOOP;
				goto done_close;
			}
			if (!S_ISDIR(sb.st_mode)) {
				fprintf(stderr,"Existing segment not a directory: %s \n", buf);
				res = -ENOTDIR;
				goto done_close;
			}
		}

		/* Yay, segment is ready for us to step into */
		int next_fd;
		if ((next_fd = openat(fd, segment, O_NOFOLLOW | O_CLOEXEC)) == -1) {
			fprintf(stderr,"Failed to openat(%s): %s \n", buf, strerror(errno));
			res = -errno;
			goto done_close;
		}

		close(fd);
		fd = next_fd;

		*p = '/';
		do {
			p++;
		} while(*p == '/'); //skip all slash
		segment = p;
    }

done_close:
    close(fd);
done:
    free(buf);
    return res;
}

int fs_create_file(char *name, mode_t mode)
{
	int fd = creat(name,mode);
	if (fd < 0) {
		fprintf(stderr,"create %s file failed : %s!\n",name,strerror(errno));
		return -1;
	}
	
	close(fd);
}
