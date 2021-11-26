#ifndef _COMMON_MACRO_H_
#define _COMMON_MACRO_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define SIMPLE_PRINT(fmt...) \
	do { \
		printf("<%s:%d>: ", __FUNCTION__, __LINE__); \
		printf(fmt); \
	} while (0)

#define DEBUG_PRINT(fmt...) 


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif
		
#ifndef MIN
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

#define SET_BIT(val,nr) ((val) |= (1 << (nr)))
#define CLEAR_BIT(val,nr) ((val) &= ~(1 << (nr)))


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif






