#include <stdio.h>
#include <string.h>

#include "memmap.h"


#define PHY_ADDR_DDR_END 	(0x140000000UL)
#define TOTAL_SHM_SIZE 		(0x8000000)
#define PHY_ADDR_BASE_SHM 	(PHY_ADDR_DDR_END - TOTAL_SHM_SIZE)

#define PAGE_SIZE (0x1000)


void test_read_write()
{
	int *ptr = NULL;
	void *addr = NULL;

	addr = memmap(PHY_ADDR_BASE_SHM, PAGE_SIZE);
	if (!addr) {
		printf("mmap %lx failed!\n",PHY_ADDR_BASE_SHM);
		return;
	}
	
	ptr = addr;

#if 0
	//bus error occur
	printf("<%s:%d> =============memset==============\n",__FUNCTION__,__LINE__);
	memset(addr,0,PAGE_SIZE);
#endif

	printf("<%s:%d> =============write==============\n",__FUNCTION__,__LINE__);
	*ptr = 0x505050;
	*(ptr + 256) = 0x505051;

	printf("<%s:%d> =============read==============\n",__FUNCTION__,__LINE__);
	printf("value [%x] sizeof(int)[%ld]\n",*ptr,sizeof(int));
	printf("value [%x]\n",*(ptr+256));

	memunmap(ptr);
}

void test_repeatedly_mmap()
{
	int *ptr = NULL;
	void *addr = NULL;

	addr = memmap(PHY_ADDR_BASE_SHM, PAGE_SIZE);
	if (!addr) {
		printf("mmap %lx failed!\n",PHY_ADDR_BASE_SHM);
		return;
	}

	addr = memmap(PHY_ADDR_BASE_SHM, PAGE_SIZE);
	if (!addr) {
		printf("mmap %lx failed!\n",PHY_ADDR_BASE_SHM);
	}

	memunmap(addr);
	memunmap(addr);
}

void test_multiple_mmap()
{
	int *ptr = NULL;
	void *addr1 = NULL;
	void *addr2 = NULL;
	void *addr3 = NULL;
	unsigned long phy_addr = PHY_ADDR_BASE_SHM;
	int size = PAGE_SIZE / 2;

	/*inside page*/
	addr1 = memmap(phy_addr, size);
	if (!addr1) {
		printf("mmap %lx failed!\n",phy_addr);
		return;
	}

	phy_addr += size;
	addr2 = memmap(phy_addr, size);
	if (!addr2) {
		printf("mmap %lx failed!\n",phy_addr);
		goto err_addr2;
	}
	
	/*out of page*/
	phy_addr += size;
	addr3 = memmap(phy_addr, size);
	if (!addr3) {
		printf("mmap %lx failed!\n",phy_addr);
		goto err_addr3;
	}

	memunmap(addr3);
err_addr3:
	memunmap(addr2);
err_addr2:
	memunmap(addr1);
}

int main(int argc, char *argv[])
{
	printf("<%s:%d> =============test_repeatedly_mmap==============\n",__FUNCTION__,__LINE__);
	test_repeatedly_mmap();
	printf("<%s:%d> =============test_read_write==============\n",__FUNCTION__,__LINE__);
	test_read_write();
	printf("<%s:%d> =============test_multiple_mmap==============\n",__FUNCTION__,__LINE__);
	test_multiple_mmap();

	return 0;
}

