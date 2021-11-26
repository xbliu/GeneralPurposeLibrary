#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

static inline uint64_t get_time_ns(void) //ns
{
    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC,&ts);
	return (uint64_t)(ts.tv_sec*1000000000+ts.tv_nsec);
}

static inline uint64_t get_time_us(void) //us
{
    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC,&ts);
	return (uint64_t)(ts.tv_sec*1000000+ts.tv_nsec/1000);
}

static inline uint64_t get_time(void) //ms
{
    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC,&ts);
	return (uint64_t)(ts.tv_sec*1000+ts.tv_nsec/1000000);
}

static inline uint32_t iclock()
{
	return (uint32_t)(get_time() & 0xfffffffful);
}

static inline int set_time(uint64_t milliseconds)
{
	struct timeval tv;

	tv.tv_sec = milliseconds/1000;
	tv.tv_usec = (milliseconds%1000)*1000;
	return settimeofday(&tv,NULL);
}

static inline uint64_t getCurrentTime() //us
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}


uint64_t get_sys_time_ns();
uint64_t get_sys_time_us();
uint64_t get_sys_time_ms();
uint64_t get_current_time_us();


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif






