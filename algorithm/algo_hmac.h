#ifndef ALGO_HMAC_H_
#define ALGO_HMAC_H_

typedef struct {
    char *algo;
    char *key;
    char *input;
} algo_hmac_t;

int hmac_encode(algo_hmac_t *algo_hmac,char **result);

#endif



