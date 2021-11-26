#include "packet_buffer.h"
#include "common_macro.h"
#include "log_adapter.h"

#include <stdio.h>
#include <stdlib.h>


int packet_buffer_update_header(packet_buffer_t *packet)
{
	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	packet->index = 0;
	packet->req_len = packet->header_len;
	packet->buf = packet->header_buffer;
	packet->state = BUFFER_STATE_HEADER;
	return 0;
}

int packet_buffer_init(packet_buffer_t *packet, int header_len)
{
	char *header = calloc(1,sizeof(char)*header_len);
	if (!header) {
		LOG_ERROR(LOG_MOD_UTILS, "no mem for packet header!\n");
		return -1;
	}

	packet->body = NULL;
	packet->body_len = 0;
	packet->header_len = header_len;
	packet->header_buffer = header;
	return packet_buffer_update_header(packet);
}

int packet_buffer_destroy(packet_buffer_t *packet)
{
	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return 0;
	}

	if (packet->header_buffer) {
		free(packet->header_buffer);
		packet->header_buffer = NULL;
	}

	if (packet->body) {
		free(packet->body);
		packet->body = NULL;
	}

	packet->header_len = 0;
	packet->body_len = 0;
	packet->req_len = 0;
	packet->buf = NULL;
	return 0;
}


int packet_buffer_update_index(packet_buffer_t *packet, int len)
{
	int ret = 0;

	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	//LOG_DBG(LOG_MOD_UTILS, "len[%d %d %d %d] \n", len, packet->index, packet->header_len, packet->req_len);

	packet->index += len;
	/*remaining length: next request*/
	if (BUFFER_STATE_HEADER == packet->state) {
		packet->req_len = packet->header_len - packet->index;
	} else if (BUFFER_STATE_BODY == packet->state) {
		packet->req_len = packet->body_len - packet->index;
	}

	if (packet->req_len > 0) {
		return RECV_STATE_IN_PROGRESS;
	}

	if (packet->req_len < 0) {
		LOG_ERROR(LOG_MOD_UTILS, "some error occur!\n");
		packet->req_len = 0;
	}

	if (BUFFER_STATE_BODY == packet->state) {
		ret = RECV_STATE_BODY_COMPLETE;
	} else if (BUFFER_STATE_HEADER == packet->state) {
		ret = RECV_STATE_HEADER_COMPLETE;
	}

	return ret;
}

int packet_buffer_update_body(packet_buffer_t *packet, int body_len)
{
	void *ptr = NULL;

	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	if (body_len > packet->body_len) {
		ptr = realloc(packet->body,body_len);
		if (!ptr) {
			LOG_ERROR(LOG_MOD_UTILS, "no mem realloc for body data!\n");
			return -1;
		}
		
		packet->body = ptr;
		packet->buf = packet->body;
	}
	packet->index = 0;
	packet->req_len = body_len;
	packet->body_len = body_len;
	packet->state = BUFFER_STATE_BODY;

	return 0;
}

inline char *packet_buffer_get_buf(packet_buffer_t *packet)
{
	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	return (packet->buf + packet->index);
}

inline int packet_buffer_get_len(packet_buffer_t *packet)
{
	if (!packet) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	return packet->index;
}

