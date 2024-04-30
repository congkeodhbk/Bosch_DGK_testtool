/**
	Standard debug utils
	
	HieuNT
	12/7/2016
**/


#ifndef _DEBUG_USER_
#define _DEBUG_USER_
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "project_conf.h"
typedef struct 
{
	void (* console_flush)(void);
	uint32_t (* console_available)(void);
	int32_t (* console_get_char)(char *c);
	void (* console_send_char)(char c);
	void (* console_send_string)(char *buf, uint32_t size);
}debug_bsp_t;

//#define DEBUG_USE_RTOS
#define DEBUG_BUFF_MAX_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif


void user_debug_init(debug_bsp_t bsp);
void user_debug_print(int level, const char* module, int line, const char* fmt, ...);
void user_debug_print_error(const char* module, int line, int ret);
void user_debug_print_exact(const char* fmt, ...);

#define __check_cmd_at_pos(x,p) (strncmp(&result[p], x, strlen(x)) == 0)
#define __check_cmd(x) __check_cmd_at_pos(x,0)
#define __param_pos(x)	((char *)(&result[strlen(x)]))	
int console_read(char **result);

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
															if (message) return message; } while (0)
extern int tests_run;

#define __check(message, test) do {if (!(test)){ error(message); return false;}} while(0)															
#if __DEBUG__ > 0
#ifndef __MODULE__
#error "__MODULE__ must be defined"
#endif
#endif

#if __DEBUG__ >= 1
#define error(...) do{ user_debug_print(1, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#define ERROR(...) do{ user_debug_print(1, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#else
#define error(...)
#define ERROR(...) 
#endif

#if __DEBUG__ >= 2
#define WARN(...) do{ user_debug_print(2, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#else
#define WARN(...) 
#endif

#if __DEBUG__ >= 3
#define info(...) do{ user_debug_print(3, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#define INFO(...) do{ user_debug_print(3, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#define LOG(...) do{ user_debug_print(3, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#else
#define info(...) 
#define INFO(...) 
#define LOG(...) 
#endif

#if __DEBUG__ >= 4
#define DEBUG(...) do{ user_debug_print(4, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#define DEBUGX(...) do{ user_debug_print_exact(__VA_ARGS__); }while(0)
// Backward compatible
#define debug(...) do{ user_debug_print(4, __MODULE__, __LINE__, __VA_ARGS__); }while(0)
#define debugx(...) do{ user_debug_print_exact(__VA_ARGS__); }while(0)
#else
#define DEBUG(...) 
#define DEBUGX(...) 
// Backward compatible
#define debug(...) 
#define debugx(...) 
#endif

#ifdef __cplusplus
}
#endif
 
#endif /* _DEBUG_USER_ */
