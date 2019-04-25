#include "algo_hmac.h"

#include <string.h>

#include <openssl/hmac.h>


int hmac_encode(algo_hmac_t *hmac,char **result)
{
    unsigned char *output = NULL;
    unsigned int output_length = 0;
    char *mdstr = NULL;
    
    if (!hmac || !result || (hmac && (!hmac->algo || !hmac->key ||!hmac->input))) {
        fprintf(stderr,"param havs null value!\n");
        return -1;
    }
    
    const EVP_MD * engine = NULL;
    if(strcasecmp("sha512", hmac->algo) == 0) {
        engine = EVP_sha512();
    } else if(strcasecmp("sha256", hmac->algo) == 0) {
        engine = EVP_sha256();
    } else if(strcasecmp("sha1", hmac->algo) == 0) {
        engine = EVP_sha1();
    } else if(strcasecmp("md5", hmac->algo) == 0) {
        engine = EVP_md5();
    } else if(strcasecmp("sha224", hmac->algo) == 0) {
        engine = EVP_sha224();
    } else if(strcasecmp("sha384", hmac->algo) == 0) {
        engine = EVP_sha384();
    } else if(strcasecmp("sha", hmac->algo) == 0) {
        engine = EVP_sha();
    } else {
        fprintf(stderr,"Algorithm %s is not supported by this program!",hmac->algo);
        return -1;
    }
    
    output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
    mdstr = (char*)malloc(EVP_MAX_MD_SIZE*2);
    
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, hmac->key, strlen(hmac->key), engine, NULL);
    HMAC_Update(&ctx, (unsigned char*)hmac->input, strlen(hmac->input)); // input is OK; &input is WRONG !!!
    
    HMAC_Final(&ctx, output, &output_length);
    HMAC_CTX_cleanup(&ctx);
    
    
    for (int i = 0; i < output_length; i++) {
        sprintf(&mdstr[i*2], "%02x", (unsigned int)output[i]);
    }
    
    *result = strdup(mdstr);
    
    free(output);
    free(mdstr);
    
    return 0;
}

