#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ipc_test_common.h"


int main(int argc, char *argv[])
{
	fprintf(stderr,"#######################################\n");
	fprintf(stderr,"Host side: api sequence testing!\n");
	fprintf(stderr,"#######################################\n");
	
	fprintf(stderr,"create!\n");
	void * handle = ipc_lock.create(argv[1]);
	if (!handle) {
		fprintf(stderr,"ipc lock init failed!\n");
		return 0;
	}
	
	fprintf(stderr,"lock!\n");
	ipc_lock.lock(handle);
	fprintf(stderr,"unlock!\n");
	ipc_lock.unlock(handle);
	fprintf(stderr,"destroy!\n");
	ipc_lock.destroy(handle);
	
	
	fprintf(stderr,"#######################################\n");
	fprintf(stderr,"Host side: get the lock first!\n");
	fprintf(stderr,"#######################################\n");
	
	fprintf(stderr,"create!\n");
	handle = ipc_lock.create(argv[1]);
	if (!handle) {
		fprintf(stderr,"ipc lock init failed!\n");
		return 0;
	}
	
	fprintf(stderr,"lock!\n");
	ipc_lock.lock(handle);
	fprintf(stderr,"sleep 30!\n");
	sleep(30);
	
	fprintf(stderr,"unlock!\n");
	ipc_lock.unlock(handle);

	sleep(10);	
	
	
	if (argv[2] == NULL) {
		fprintf(stderr,"#######################################\n");
		fprintf(stderr,"Host side: process abnormal exit!\n");
		fprintf(stderr,"#######################################\n");
		
		fprintf(stderr,"lock!\n");
		ipc_lock.lock(handle);
	} else {
		fprintf(stderr,"#######################################\n");
		fprintf(stderr,"Host side: process normal destroy!\n");
		fprintf(stderr,"#######################################\n");
		
		ipc_lock.destroy(handle);
		sleep(30);
	}
	
	return 0;
}

