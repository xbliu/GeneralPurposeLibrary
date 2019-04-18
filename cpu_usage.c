#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <math.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <time.h>
#define _GNU_SOURCE   
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#include <signal.h>

typedef struct {
	int id;  //cpu id
	int run;
	long int total_time;
	long int idle_time;
	clock_t *base_time; //sleep reference Unit
	FILE *fp;
} cpus_info_t;

typedef struct {
	cpus_info_t *cpu;
	clock_t base_time;
	double rate;
} single_usage_t;


static single_usage_t *m_usage;
static int m_wait = 0;

//typedef long long int int64_t;
const int NUM_THREADS = 1; //CPU core nums
int INTERVAL = 100;
int cpuinfo = 50; //CPU utilization rate

// time unit is "ms" 
int64_t get_tick_count() 
{ 
	struct timespec now; 
	int64_t sec, nsec; 
	clock_gettime(CLOCK_MONOTONIC, &now); 
	sec = now.tv_sec; 
	nsec = now.tv_nsec; 
	return sec * 1000 + nsec / 1000000; 
}


// 设置进程在哪个cpu上运行
void set_cpu(int id)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        fprintf(stderr, "warning: could not set CPU affinity\n");
    }
}

int get_cpu_num()
{
	return get_nprocs();
}

void cpus_refresh(cpus_info_t *cpus)
{
	char line[128] = {0};
	char cpu[32] = {0};
	long int user = 0;
	long int nice = 0;
	long int sys = 0;
	long int idle = 0;
	long int iowait = 0;
	long int irq = 0;
	long int softirq = 0;
	int id = 0;
	int total = 0;
	
	rewind(cpus->fp);
	fflush(cpus->fp);
	
	while (fgets(line,sizeof(line),cpus->fp)) {
		sscanf(line,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
		if (!strstr(cpu,"cpu")) {
			break;
		}
		
		if ('\0' == cpu[3]) {
			if (-1 != cpus->id) {
				continue;
			}
			cpus->total_time = user + nice + sys + idle + iowait + irq + softirq;
			cpus->idle_time = idle;
			break; //only total cpus stat
		} else {
			id = atoi(cpu+3);
		}
		
		if (id == cpus->id) {
			cpus->total_time = user + nice + sys + idle + iowait + irq + softirq;
			cpus->idle_time = idle;
			break; //only <id> cpu stat
		}
	}
}

void cpu_usage(cpus_info_t *cpu, double rate)
{
	double scale = 0;
	cpus_info_t old;
	clock_t busy_time = 0;
	int clock_per_us = 0;
	
	set_cpu(cpu->id);
	fprintf(stderr,"cpu id:%d\n",cpu->id);
	
	//clock_t start;
	long int start = 0;
	while (cpu->run) {
		cpus_refresh(cpu);
		old = *cpu;
		// idle loop
		usleep(*cpu->base_time);
		cpus_refresh(cpu);
		/* 
		*** Phase I.  cpu idle usage for others : (idle_new - idle_old) * 100.0 / (total_new - total_old)
		*** Phase II. busy time : CPU occupancy rate is 100%
		*/
		busy_time = (cpu->idle_time - old.idle_time) * 100.0 / (100 -rate) - (cpu->total_time - old.total_time);
		clock_per_us = *cpu->base_time / (cpu->total_time - old.total_time);
		//fprintf(stderr,"id:%d idle time:%ld total_time:%ld CLOCKS_PER_SEC:%ld rate:%lf base_time:%ld clock_per_us:%d\n", \
				cpu->id,(cpu->idle_time - old.idle_time),(cpu->total_time - old.total_time),CLOCKS_PER_SEC,rate,*cpu->base_time, \
			    clock_per_us);
		//start = times(NULL);
		start = get_tick_count();
		// busy loop
		busy_time *= ceil(clock_per_us) / 1000;
		while (get_tick_count() - start <= busy_time)
			;
	}
}

void *single_cpu_run(void *arg) 
{
	single_usage_t *usage = (single_usage_t *)arg;
	FILE *fp = fopen("/proc/stat","r");
	if (!fp) {
		fprintf(stderr,"can't open /proc/stat!\n");
		return NULL;
	}
	usage->cpu->fp = fp; //for no lock
	cpu_usage(usage->cpu, usage->rate);
	return arg;
}

void int_handler(int s){
	int cpu_num = get_cpu_num();
	int i = 0;
	
	fprintf(stderr,"Caught signal %d\n",s);
	
	for (i=0; i<cpu_num; i++) {
		m_usage[i].cpu->run = 0;
		fclose(m_usage[i].cpu->fp);
	}
	
	m_wait = 0;
}

void *forced_occupancy_half(void *arg)
{
	int id = (int)arg;
	int busy_time = INTERVAL * cpuinfo / 100;
	int idle_time = INTERVAL - busy_time;
	int64_t start = 0;
	
	fprintf(stderr,"id:%d\n",id);
	set_cpu(id);
	
	/*
        * within INTERVAL ms, INTERVAL = busy_time + idle_time,
        * spend busy_time ms to let cpu busy,
        * spend idle_time ms top let cpu idle
        */ 
	while (1) { 
		start = get_tick_count();
		while((get_tick_count() - start) <= busy_time); 
		usleep(idle_time * 1000); 
	}
	
	return arg;
}

#if 1
int main(int argc, char *argv[]) 
{
	int i = 0;
	clock_t base_time = 0;
	FILE* fp = NULL;	
	struct sigaction sigint_handler;
	cpus_info_t *cpus = NULL;
	cpus_info_t total_cpu = {0};
	cpus_info_t last_cpu = {0};
	int cpu_num = get_cpu_num();
	int precision = 1;
	int diff_value = 0;
	int last_diff_value = 100;
	int quick_grow = 1;
	int step = 1; //ms
	double actual_rate = 0;
	double request_rate = 50;
	
	
	base_time = 100000*cpu_num; //us
	if (argc > 2) {
		request_rate = atof(argv[1]);
		base_time = atoi(argv[2]);
	}
	fprintf(stderr,"req rate:%f base_time:%ld\n",request_rate,base_time);
	
	sigint_handler.sa_handler = int_handler;
	sigemptyset(&sigint_handler.sa_mask);
	sigint_handler.sa_flags = 0;
	sigaction(SIGINT, &sigint_handler, NULL);
	
	fprintf(stderr,"cpu num:%d\n",cpu_num);
	cpus = (cpus_info_t *)calloc(cpu_num,sizeof(cpus_info_t)+sizeof(single_usage_t));
	if (!cpus) {
		fprintf(stderr,"alloc cpus info failed!\n");
		return 0;
	}
	m_usage = (single_usage_t *)(cpus + cpu_num);
	
	m_wait = 1;
	pthread_t thread;
	for (i=0; i<cpu_num; i++) {
		cpus[i].id = i;
		//cpus[i].fp = fp;
		cpus[i].run = 1;
		cpus[i].base_time = &base_time;
		m_usage[i].cpu = &cpus[i];
		m_usage[i].rate = request_rate;
		
		pthread_create(&thread,NULL,single_cpu_run,&m_usage[i]);
	}
	
	fp = fopen("/proc/stat","r");
	if (!fp) {
		fprintf(stderr,"can't open /proc/stat!\n");
		return 0;
	}

#if 0
	//Precision Adjustment algorithm : it's poor
	total_cpu.id = -1;
	total_cpu.fp = fp;
	do {
		cpus_refresh(&total_cpu);
		last_cpu = total_cpu;
		usleep(1000*1000*3);
		cpus_refresh(&total_cpu);
		actual_rate = 100 - ((total_cpu.idle_time - last_cpu.idle_time) * 100 / (total_cpu.total_time - last_cpu.total_time));
		diff_value = abs(request_rate - actual_rate);
		fprintf(stderr,"actual rate:%d dv:%d base_time:%ld quickgrow:%d\n",actual_rate,diff_value,base_time,quick_grow);
		if (diff_value > precision) { 
			if (diff_value > last_diff_value) { //too much growth using a stepping approach
				if (quick_grow) {
					base_time >>= 2;
				}
				fprintf(stderr,"slow grow base_time:%ld\n",base_time);
				quick_grow = 0;
			}
			base_time = quick_grow ? (base_time << 2) : (base_time + step);
			fprintf(stderr,"grow base_time:%ld\n",base_time);
		}
		last_diff_value = diff_value;
	} while(m_wait);
#else
	do {
		usleep(1000*1000*3);
	} while (m_wait);
#endif

	fprintf(stderr,"cleanup it..........\n");
	fclose(fp);
	free(cpus);
	
	return 0;
}
#else
int main(int argc, char *argv[])
{
	int i = 0;
	int cpu_num = get_cpu_num();
	pthread_t thread;
	
	if (2 == argc) {
		cpuinfo = atoi(argv[1]);
	}

	for (i=0; i<cpu_num; i++) {
		pthread_create(&thread,NULL,forced_occupancy_half,(void *)i);
	}

	while (1) {
		usleep(3000000);
	}
	return 0;
}
#endif
