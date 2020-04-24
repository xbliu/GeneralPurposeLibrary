#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ipc_test_common.h"

int main(int argc, char *argv[])
{	
	void * handle = NULL;
//#ifdef IPC_MUTEX	
	fprintf(stderr,"#######################################\n");
	fprintf(stderr,"slave side: api sequence testing!\n");
	fprintf(stderr,"#######################################\n");
	
	fprintf(stderr,"create!\n");
	handle = ipc_lock.create(argv[1]);
	if (!handle) {
		fprintf(stderr,"ipc lock init failed!\n");
		return 0;
	}
	
	fprintf(stderr,"destroy!\n");
	ipc_lock.destroy(handle);
//#endif
	
	fprintf(stderr,"#######################################\n");
	fprintf(stderr,"slave side: wait host mutex!\n");
	fprintf(stderr,"#######################################\n");
	
	fprintf(stderr,"create!\n");
	handle = ipc_lock.create(argv[1]);
	if (!handle) {
		fprintf(stderr,"ipc lock init failed!\n");
		return 0;
	}
	
	fprintf(stderr,"lock!\n");
	ipc_lock.lock(handle);
	fprintf(stderr,"unlock!\n");
	ipc_lock.unlock(handle);
	
	sleep(20);
	
	
	fprintf(stderr,"#######################################\n");
	fprintf(stderr,"slave side: host abnormal exit,deadlock???\n");
	fprintf(stderr,"#######################################\n");
	
	fprintf(stderr,"lock!\n");
	ipc_lock.lock(handle);
	
	fprintf(stderr,"lock!\n");
	ipc_lock.lock(handle);
	
	fprintf(stderr,"destroy!\n");
	ipc_lock.destroy(handle);
	
	
	return 0;
}
