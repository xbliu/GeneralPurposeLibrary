#include "stats.h"
#include "common_macro.h"
#include "log_adapter.h"

#include <stdio.h>
#include <string.h>


static void flush_batch(stats_t *stats)
{
    if (!stats) {
        LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
        return;
    }

    if (!stats->nr_batch) {
        return;
    }

    if (!stats->nr_samples) {
        stats->average = stats->batch / stats->nr_batch;
    } else {
        stats->average = ((stats->average * stats->nr_samples) + stats->batch) /  \
                            (stats->nr_samples + stats->nr_batch);
    }

    stats->nr_samples += stats->nr_batch;
    stats->nr_batch = stats->batch = 0;
}


int stats_init(stats_t *stats)
{
    if (!stats) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
        return -1;
    }

    memset(stats,0,sizeof(*stats));
    stats->max_batch = 32;
    stats->min = -1UL;

    return 0;
}

int stats_add(stats_t *stats, int value)
{
    if (!stats) {
        LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
        return -1;
    }

    stats->min = MIN(stats->min,value);
    stats->max = MAX(stats->max,value);

    if ((stats->batch + value < stats->batch) || (stats->nr_batch + 1 == stats->max_batch)) {
        flush_batch(stats);
    }

    stats->batch += value;
    stats->nr_batch++;

    return 0;
}

int stats_set_max_batch(stats_t *stats, int max_batch)
{
    if (!stats) {
        LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
        return -1;
    }

    stats->max_batch = max_batch;
    return 0;
}

void stats_print(stats_t *stats)
{
    if (!stats) {
        LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
        return;
    }

    flush_batch(stats);
    LOG_INFO(LOG_MOD_UTILS, "min[%u] max[%u] average[%d] nr_samples[%d] \n", \
            stats->min, stats->max, stats->average, stats->nr_samples);
}

