/**
	Standard debug utils
	
	MinhTh
	12/7/2016
**/

#include "debug.h"
#include <stdarg.h>
#include <stdio.h>


#ifdef DEBUG_USE_RTOS
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "semphr.h"
SemaphoreHandle_t dbg_semHandler = NULL;
#endif

static debug_bsp_t debug_bsp = {0};

static char buffer[DEBUG_BUFF_MAX_SIZE];
static char consoleBuffer[DEBUG_BUFF_MAX_SIZE];
// thay the debug neu muon
// function user_debug_init() : khoi dong debug
// function uart_printf(): chinh sua ham xuat ra ngoai
// function uart_vprintf(): chinh sau ham xuat ra ngoai

void user_debug_init(debug_bsp_t bsp)
{
#ifdef DEBUG_USE_RTOS
	dbg_semHandler = xSemaphoreCreateMutex();
#endif
	debug_bsp = bsp;
}


int console_read(char **result)
{
	static int i = 0;

	while (debug_bsp.console_available())
	{
		char inChar;
		uint32_t len = 1;
		debug_bsp.console_get_char(&inChar);
		if (inChar == '\r' || inChar == '\n')
		{
			consoleBuffer[i] = '\0';
			len = i;
			i = 0;
			*result = consoleBuffer;
			// console_flush();
			return len;
		}
		else if (inChar != '\n')
		{
			consoleBuffer[i] = inChar;
			i++;
			if (i == (DEBUG_BUFF_MAX_SIZE - 1))
			{
				consoleBuffer[i] = '\0';
				len = i;
				i = 0;
				*result = consoleBuffer;
				// console_flush();
				return len;
			}
		}
	}
	return 0;
}

static void uart_printf(const char *fmt, ...)
{
	uint32_t len;
	va_list vArgs;
	va_start(vArgs, fmt);
	len = vsprintf((char *)&buffer[0], (char const *)fmt, vArgs);
	va_end(vArgs);

	// xuat du lieu ra ben ngoai
	// HmSerialUart8_Transmit(&debug_user_serial_uart_handle, buffer, len);
	debug_bsp.console_send_string(buffer, len);
}

// chinh sua cai nay
static void uart_vprintf(const char *fmt, va_list vArgs)
{
	uint32_t len;
	len = vsprintf((char *)&buffer[0], (char const *)fmt, vArgs);

	// HmSerialUart8_Transmit(&debug_user_serial_uart_handle, buffer, len);
	debug_bsp.console_send_string( buffer, len);
}

/* ############### Actual debug redirect ################# */
#define __debug_printf(fmt, ...) uart_printf(fmt, __VA_ARGS__)
#define __debug_vprintf(fmt, vArgs) uart_vprintf(fmt, vArgs)

/* ------------------- MAIN DEBUG I/O -------------- */
void user_debug_print(int level, const char *module, int line, const char *fmt, ...)
{
#ifdef DEBUG_USE_RTOS
	// osSemaphoreWait(dbgSem_id , osWaitForever);
	xSemaphoreTake(dbg_semHandler, (TickType_t)osWaitForever);
#endif

	switch (level)
	{
	case 1:
		// "->[ERROR](module:line): "
		__debug_printf("->[ERROR](%s:%d): ", module, line);
		break;
	case 2:
		// "->[WARN](module:line): "
		__debug_printf("->[WARN](%s:%d): ", module, line);
		break;
	case 3:
		// "->[INFO](module:line): "
		__debug_printf("->[INFO](%s:%d): ", module, line);
		break;
	case 4:
	default:
		// Don't need to print DEBUG :P
		__debug_printf("->(%s:%d): ", module, line);
		break;
	}

	va_list vArgs;
	va_start(vArgs, fmt);
	__debug_vprintf((char const *)fmt, vArgs);
	va_end(vArgs);

#ifdef DEBUG_USE_RTOS
	// osSemaphoreRelease(dbgSem_id);
	xSemaphoreGive(dbg_semHandler);
#endif
}

void dbg_error(const char *module, int line, int ret)
{
#ifdef DEBUG_USE_RTOS
	// osSemaphoreWait(dbgSem_id , osWaitForever);
	xSemaphoreTake(dbg_semHandler, (TickType_t)osWaitForever);
#endif

	__debug_printf("[RC] Module %s - Line %d : Error %d\n", module, line, ret);

#ifdef DEBUG_USE_RTOS
	// osSemaphoreRelease(dbgSem_id);
	xSemaphoreGive(dbg_semHandler);
#endif
}

void user_debug_print_exact(const char *fmt, ...)
{
#ifdef DEBUG_USE_RTOS
	// osSemaphoreWait(dbgSem_id , osWaitForever);
	xSemaphoreTake(dbg_semHandler, (TickType_t)osWaitForever);
#endif

	va_list vArgs;
	va_start(vArgs, fmt);
	__debug_vprintf((char const *)fmt, vArgs);
	va_end(vArgs);

#ifdef DEBUG_USE_RTOS
	// osSemaphoreRelease(dbgSem_id);
	xSemaphoreGive(dbg_semHandler);
#endif
}

