
all: ipc_mutex_test ipc_semsv_test
	@echo "##########compile over!##########"
	
ipc_mutex_test:
	gcc -o ipc_mutex_host ipc_mutex.c ipc_test_host.c -DIPC_MUTEX -lrt -lpthread -g 
	gcc -o ipc_mutex_slave ipc_mutex.c ipc_test_slave.c -DIPC_MUTEX -lrt -lpthread -g
	
ipc_semsv_test:
	gcc -o ipc_semsv_host ipc_semsv.c ipc_test_host.c -DIPC_SEMSV -lrt -lpthread -g 
	gcc -o ipc_semsv_slave ipc_semsv.c ipc_test_slave.c -DIPC_SEMSV -lrt -lpthread -g	
	
clean:
	rm -rf *.o ipc_mutex_host ipc_mutex_slave ipc_semsv_host ipc_semsv_slave
	
