#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFSIZE     1024*4
static unsigned int crc_table[256];

void init_crc_table(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320U ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[i] = c;
    }
}

/*crc32/mpeg-2*/
unsigned int crc32(unsigned int crc, unsigned char *buffer, unsigned int size)
{
    unsigned int i;
    for (i = 0; i < size; i++) {
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ;
}

unsigned int calc_crc(unsigned char *buf, unsigned int size)
{
    unsigned int crc = 0xffffffff;
    return crc32(crc, buf, size);
}

unsigned int lut_crc32(unsigned int crc, const unsigned char *buf, size_t len) {
    unsigned int table[256];
    int have_table = 0;

    /* This check is not thread safe; there is no mutex. */
    if(have_table == 0) {
        /* Calculate CRC table. */
        for(int i = 0; i < 256; i++) {
            unsigned int rem = i; /* remainder from polynomial division */
            for(int j = 0; j < 8; j++) {
                if(rem & 1) {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                } else
                    rem >>= 1;
            }
            table[i] = rem;
        }
        have_table = 1;
    }

    crc = ~crc;
    const unsigned char *q = buf + len;
    for(const unsigned char *p = buf; p < q; p++) {
        unsigned char octet = *p; /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}


unsigned int *crc32_alloc_init_table(void)
{
	unsigned int c = 0;
    unsigned int i = 0, j = 0;
	unsigned int *table = NULL;

	table = calloc(1, sizeof(unsigned int)*256);
	if (!table) {
		printf("<%s:%d> no mem alloc!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320U ^ (c >> 1);
            else
                c = c >> 1;
        }
        table[i] = c;
    }

	return table;
}

/*crc32*/
unsigned int crc32_get(unsigned int *table, unsigned char *buf, unsigned int size)
{
	unsigned int crc = 0xffffffff;
	unsigned int i = 0;

    for (i = 0; i < size; i++) {
        crc = table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }

    return ~crc ;
}

int crc32_destroy(unsigned int *table)
{
	if (table) {
		free(table);
	}

	return 0;
}

