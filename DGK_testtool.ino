
#define __DEBUG__ 4
#define __MODULE__ "main"

#include "debug.h"

void console_command_handle(char *result)
{
	if(__check_cmd("hello"))
	{
		debug("hi there\n");
	}
}

void console_flush(void)
{
  Serial.flush();
}

uint32_t console_available(void)
{
  return Serial.available();
}
int32_t console_get_char(char *c)
{
  *c=Serial.read();
  return 1;
}
void console_send_char(char c)
{
  Serial.write(c);
}
void console_send_string(char *buf, uint32_t size)
{
  Serial.write(buf, size);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  debug_bsp_t debug_bsp;
	debug_bsp.console_flush = console_flush;
	debug_bsp.console_available = console_available;
	debug_bsp.console_get_char = console_get_char;
	debug_bsp.console_send_char = console_send_char;
	debug_bsp.console_send_string = console_send_string;
  user_debug_init(debug_bsp);
  debug("BOSCH DGK test tool\n");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  static char *result;
  if(console_read(&result) > 0)
	{
		console_command_handle(result);
	}
}
