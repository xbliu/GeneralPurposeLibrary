#include <stdint.h>
#include <time.h>
#include <sys/time.h>


uint64_t get_sys_time_ns()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC,&ts);
    
    return (uint64_t)(((uint64_t)(ts.tv_sec))*1000000000 +(uint64_t)(ts.tv_nsec));

}


uint64_t get_sys_time_us()
{
	return (get_sys_time_ns() / 1000);
}

uint64_t get_sys_time_ms()
{
	return (get_sys_time_us() / 1000);
}

uint64_t get_current_time_us()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

