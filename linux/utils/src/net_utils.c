#include "log_adapter.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <stdio.h>


int tcp_socket_listen(char *ip_addr, int ip_port, int listen_num, int *fd)
{
	int ret = 0;
	int socket_fd = -1;
	struct sockaddr_in srv_addr;
	int reuseaddr = 1;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == socket_fd) {
        LOG_ERROR(LOG_MOD_UTILS, "create socket failed: ip info[%s %d]!\n", ip_addr, ip_port);
		ret = -1;
        goto err_out;
    }

	srv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &srv_addr.sin_addr.s_addr);
    srv_addr.sin_port = htons(ip_port);

	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	ret = bind(socket_fd, (const struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
	if(-1 == ret) {
        LOG_ERROR(LOG_MOD_UTILS, "bind server socket failed!\n");
		goto err_bind;
    }
 	
	ret = listen(socket_fd, listen_num);
    if(-1 == ret) {
        LOG_ERROR(LOG_MOD_UTILS, "listen server socket failed!\n");
		goto err_bind;
    }

	*fd = socket_fd;
	return 0;

err_bind:
	*fd = -1;
	close(socket_fd);
err_out:
	return ret;
}

