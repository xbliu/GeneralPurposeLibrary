
all: udp_client sniffer_test
	@echo "##########compile over!##########"
	
udp_client:
	gcc -o udp_client udp_client.c  udp_test.c -g

sniffer_test:
	gcc -o sniffer_test sniffer_framework.c  sniffer_test.c ../pthread/pthread_model.c -g -I../ -lpthread
	
clean:
	rm -rf *.o udp_client sniffer_test
	
