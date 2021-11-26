#ifndef _HEADER_EXAMPLE_H_
#define _HEADER_EXAMPLE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


typedef enum {
	RECV_STATE_IN_PROGRESS = 0,
	RECV_STATE_HEADER_COMPLETE,
	RECV_STATE_BODY_COMPLETE,
	RECV_STATE_BODY_BUTT,
} recv_state_e;

typedef enum {
	BUFFER_STATE_HEADER,
	BUFFER_STATE_BODY,
	BUFFER_STATE_BUTT,
} buffer_state_e;

typedef struct {
	int (*read)(void *handle, void *buffer, int len);
	int (*parse_header)(void *handle, void *buffer, int len);
	int (*parse_body)(void *handle, void *buffer, int len);
} buffer_operations_t;

typedef struct {
	int header_len;
	char *header_buffer;
	int body_len;
	char *body;

	int index;
	int req_len;
	char *buf;
	buffer_state_e state;

	void *handle;
	buffer_operations_t ops; //reserved
} packet_buffer_t;


int packet_buffer_update_header(packet_buffer_t *packet);
int packet_buffer_init(packet_buffer_t *packet, int header_len);
int packet_buffer_destroy(packet_buffer_t *packet);
int packet_buffer_update_index(packet_buffer_t *packet, int len);
int packet_buffer_update_body(packet_buffer_t *packet, int body_len);
char *packet_buffer_get_buf(packet_buffer_t *packet);
int packet_buffer_get_len(packet_buffer_t *packet);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif







