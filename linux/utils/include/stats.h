#ifndef _STATS_H_
#define _STATS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#include <stdint.h>


typedef struct {
    uint64_t min;
    uint64_t max;
    int64_t average;
    int nr_samples;

    uint64_t batch; /*the total value of this statistic*/
    int nr_batch; /*number of this statistic*/
    int max_batch;
} stats_t;


int stats_init(stats_t *stats);
int stats_add(stats_t *stats, int value);
int stats_set_max_batch(stats_t *stats, int max_batch);
void stats_print(stats_t *stats);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif







