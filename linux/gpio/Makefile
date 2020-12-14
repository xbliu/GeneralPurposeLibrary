all: gpio_base_test
	@echo "##########compile over!##########"
	
gpio_base_test:
	/opt/hisi-linux/x86-arm/aarch64-himix100-linux/bin/aarch64-himix100-linux-gcc -o gpio_base_test gpio_base.c gpio_base_test.c gpio_ctrl.c -g 
	
clean:
	rm -rf *.o gpio_base_test
	
