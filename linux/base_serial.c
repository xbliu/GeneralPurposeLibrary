#include <termios.h>

#define NELEMS(x)       (sizeof(x) / sizeof((x)[0]))


typedef struct {
    int speed;
    int baudrate;
} bd_speed_map_t;


static bd_speed_map_t m_map_table[] = {
	{ 115200, B115200 }, //commonly used,so put it first
//  { 0,      B0      },
//  { 50,     B50     },
//  { 75,     B75     },	
    { 110,    B110    },
    { 134,    B134    },
    { 150,    B150    },
    { 200,    B200    },
    { 300,    B300    },
    { 600,    B600    },
    { 1200,   B1200   },
    { 1800,   B1800   },
    { 2400,   B2400   },
    { 4800,   B4800   },
    { 9600,   B9600   },
    { 19200,  B19200  },
    { 38400,  B38400  },
    { 57600,  B57600  },
    { 230400, B230400 },
	{ 460800, B460800 },
};


static int get_baudrate(int speed)
{
    int i = 0;
	bd_speed_map_t *map = NULL;
	
    for (i = 0; i < NELEMS(m_map_table); i++) {
        map = &m_map_table[i];
        if (map->speed != speed) {
			continue;
		}
		
        return map->baudrate;
    }

    return -1;
}

static int set_data_bits(struct termios *options, int data_bits)
{
	switch (data_bits) {
		case 5:
			options->c_cflag |= CS5;
			break;
		case 6:
			options->c_cflag |= CS6;
			break;
        case 7:
            options->c_cflag |= CS7;
            break;
		default:
            fprintf(stderr, "unsupported data size:%d,use 8 bits\n",data_bits);
        case 8:
            options->c_cflag |= CS8;
            break;
    }
	
	return 0;
}

static int set_parity(struct termios *options, char parity)
{
	/* parity */
    switch (parity) {
		default:
            fprintf(stderr, "unsupported parity:%c,use none parity\n",parity);
        case 'n':   /* none parity */
        case 'N':
            options->c_cflag &= ~PARENB;  /* Clear parity enable flag */
            options->c_iflag &= ~INPCK;   /* Enable parity checking */
            break;
        case 'o':   /* odd parity */
        case 'O':
            options->c_cflag |= PARENB;   /* odd parity */
            options->c_cflag |= PARODD;
            options->c_iflag |= INPCK;    /* Enable input parity checking */
            break;
        case 'e':   /* even parity */
        case 'E':
            options->c_cflag |= PARENB;   /* Enable parity */
            options->c_cflag &= ~PARODD;
            options->c_iflag |= INPCK;    /* Enable input parity checking */
            break;
        case 's':   /* as no parity */
        case 'S':
            options->c_cflag &= ~PARENB;
            options->c_cflag &= ~CSTOPB;
            options->c_iflag |= INPCK;
            break;
    }
	
	return 0;
}

static int set_stop_bits(struct termios *options, int stop_bits)
{
	/* stopbit */
    switch (stopbits) {
		default:
            fprintf(stderr, "unsupported stop bits:%d,usb 1 stop bits\n",stop_bits);
        case 1:
            options->c_cflag &= ~CSTOPB;
            break;
        case 2:
            options->c_cflag |= CSTOPB;
            break;
    }
	
	return 0;
}

int serial_set_speed(int fd, int speed)
{
	struct termios opt;
    int baudrate = get_baudrate(speed);
	
    if (baudrate < 0) {
		printf("not found baudrate %d\n",speed);
		return -1;
	}

    tcflush(fd, TCIOFLUSH);
	
    tcgetattr(fd, &opt);
    cfsetispeed(&opt, baudrate);
    cfsetospeed(&opt, baudrate);

    if (tcsetattr(fd, TCSANOW, &opt) < 0) {
		return -1;
	}

    return 0;
}

int serial_set_databits(int fd, int data_bits)
{
	int ret = 0;
	struct termios options;
	
	ret = tcgetattr(fd, &options);
	if (0 != ret) {
		return -1;
	}
    
	set_data_bits(&options,data_bits);
	
	tcflush(fd, TCIFLUSH);  /* update the options and do it NOW */
	ret = tcsetattr(fd, TCSANOW, &options);
    if (0 != ret) {
        perror("set data bits");
        return -1;
    }
	
	return 0;
}

int serial_open(char *dev_name)
{
	int fd = 0;
	
	fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        perror("serial_open");
        return -1;
    }
	
	return fd;
}

int serial_close(int fd)
{
	if (fd < 0) {
		return 0;
	}
	
	return close(fd);
}





