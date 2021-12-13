#ifndef _LOG_ADAPTER_H_
#define _LOG_ADAPTER_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if 0
typedef enum {
	LOG_LEVEL_FALT =0,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DBG,
	LOG_LEVEL_BUTT,
} log_level_e;

typedef enum {
	LOG_MOD_SELF = 0,
	LOG_MOD_UTILS,
	LOG_MOD_VISION,
	LOG_MOD_STREAM,
	LOG_MOD_ALG_ADPT,
	LOG_MOD_TEST,
	LOG_MOD_BUTT,
} log_module_e;
#endif


#ifdef USE_CONTROL_LOG
/*****************vision control log public header********************************************/
typedef enum au_vision_loglevel_ {
	VISION_LOG_LEVEL_EMERGENCY =0,
	VISION_LOG_LEVEL_ERROR,
	VISION_LOG_LEVEL_WARN,
	VISION_LOG_LEVEL_INFO,
	VISION_LOG_LEVEL_DBG,
	VISION_LOG_LEVEL_COUNT,
} au_vision_loglevel;

typedef enum log_module_id_ {
    MODULE_LOG			= 0,
    MODULE_AUTEL_VISION,
} log_module_id;

int VisionControlLog(log_module_id log_module, au_vision_loglevel dwLogLevel, const char *sFileName, const char * sFunName, int dwLine, const char *fmt, ...);
int VisionControlLogInit(void);
int VisionControlLogDeinit(void);

/*log module*/
#define LOG_MOD_SELF 		(MODULE_LOG)
#define LOG_MOD_UTILS 		(MODULE_AUTEL_VISION)
#define LOG_MOD_MEMPOOL 	(MODULE_AUTEL_VISION)
#define LOG_MOD_VISION 		(MODULE_AUTEL_VISION)
#define LOG_MOD_STREAM 		(MODULE_AUTEL_VISION)
#define LOG_MOD_ALG_ADPT 	(MODULE_AUTEL_VISION)
#define LOG_MOD_TEST		(MODULE_AUTEL_VISION)

#define LOG_ERROR(module, fmt, ...) VisionControlLog(module, VISION_LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(module, fmt, ...) 	VisionControlLog(module, VISION_LOG_LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(module, fmt, ...) 	VisionControlLog(module, VISION_LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DBG(module, fmt, ...) 	VisionControlLog(module, VISION_LOG_LEVEL_DBG, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_INIT() VisionControlLogInit()
#define LOG_DESTROY() VisionControlLogDeinit()

#else

/*****************default use libc printf********************************************/
/*log module*/
#define LOG_MOD_SELF 		"self"
#define LOG_MOD_UTILS 		"utils"
#define LOG_MOD_MEMPOOL 	"mempool"
#define LOG_MOD_VISION 		"vision"
#define LOG_MOD_STREAM 		"stream"
#define LOG_MOD_ALG_ADPT 	"alg_adpt"
#define LOG_MOD_TEST		"test"


#define LOG_ERROR(module, fmt, ...) printf("Err module[%s] <%s:%d> " fmt, module, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(module, fmt, ...) 	printf("Warn module[%s] <%s:%d> " fmt, module, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(module, fmt, ...) 	printf("Info module[%s] <%s:%d> " fmt, module, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_DBG(module, fmt, ...) 	printf("Dbg module[%s] <%s:%d> " fmt, module, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_INIT() //nothing
#define LOG_DESTROY() //nothing

#endif


#define LOG_PRINT_ONCE_TIME(module, fmt, ...) \
	do { \
		static int times = 1; \
		if (0 == times) { \
			break; \
		} \
		times--; \
		LOG_INFO(module, fmt, ##__VA_ARGS__); \
	} while (0)

#define LOG_PRINT_TIMES(ret, count, module, fmt, ...) \
do { \
		static int times = 0; \
		static int last_ret = 0; \
		if ((last_ret == ret) && (times < count)) { \
			last_ret = ret; \
			times++; \
			break; \
		} \
		last_ret = ret; \
		LOG_ERROR(module, "total times:%d\n", times); \
		times = 0; \
		LOG_ERROR(module, fmt, ##__VA_ARGS__); \
	} while (0)

#define LOG_CHECK_RETURN_TIMES(ret, max_count, module, name, fmt, ...) \
do { \
		static int err_times = 0; \
		static int last_ret = 0; \
		if ((last_ret == ret) && (0 == last_ret)) { \
			last_ret = ret; \
			break; \
		} \
		err_times++; \
		if ((last_ret == ret) && (err_times < max_count)) { \
			last_ret = ret; \
			break; \
		} \
		LOG_ERROR(module, "name[%s] total times[%d] errcode[0x%x 0x%x]\n", name, err_times, ret, last_ret); \
		LOG_ERROR(module, fmt, ##__VA_ARGS__); \
		last_ret = ret; \
		err_times = 0; \
} while (0)

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif

