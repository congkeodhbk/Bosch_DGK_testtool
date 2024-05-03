#define __MODULE__ "DGK"
#define __DEBUG__ 4

#include <Arduino.h>
#include <EEPROM.h>
#include "debug.h"
#include "dgk.h"
#include "crc16.h"

#define OUTPUT_PIN A0

static dgk_all_config_t dgk_config;

static uint8_t state=LOW;
static uint32_t tick;
static bool dgk_active=false;
static int lowTime=0;
static int highTime=0;

static void dgk_config_save()
{
  uint8_t *config = (uint8_t *)&dgk_config;
  crc16_append(config, sizeof(dgk_all_config_t)-2);
  for(int i =0; i<sizeof(dgk_all_config_t); i++)
  {
    EEPROM.write(i, *(config+i));
  }
}

static void dgk_set_default_config()
{
  for(int i=0; i<DGK_MAX_PATTERN; i++)
  {
    dgk_config.patterns[i].lowTime = DEFAULT_PATTERN_LOW;
    dgk_config.patterns[i].highTime = DEFAULT_PATTERN_HIGH;
  }
  dgk_config.realScg.lowTime=DEFAULT_REALSCG_LOW;
  dgk_config.realScg.highTime=DEFAULT_REALSCG_HIGH;
  dgk_config_save();
  debug("Set default value\n");
}

void dgk_init()
{
  uint8_t *config = (uint8_t *)&dgk_config;
  for(int i = 0; i<sizeof(dgk_all_config_t); i++)
  {
    *(config + i) = EEPROM.read(i);
  }
  if(crc16_frame_check(config, sizeof(dgk_all_config_t)))
  {
    debug("Config is correct\n");
  }
  else
  {
    debug("Config error\n");
    dgk_set_default_config();
  }
  digitalWrite(OUTPUT_PIN, LOW);
  pinMode(OUTPUT_PIN, OUTPUT);
}

static void dgk_show_config()
{
  for(int i=0; i<DGK_MAX_PATTERN; i++)
  {
    debug("Pattern %d: lowTime: %d, highTime: %d\n", i, dgk_config.patterns[i].lowTime, dgk_config.patterns[i].highTime);
  }
  debug("Real SCG lowTime: %d, highTime: %d\n", dgk_config.realScg.lowTime, dgk_config.realScg.highTime);
}

static void output_set(uint8_t level)
{
  debug("DGK %s\n", (level==LOW)?"HIGH":"LOW");
  digitalWrite(OUTPUT_PIN, level);
}

static void activate_output(int low, int high)
{
  dgk_active=true;
  lowTime=low;
  highTime=high;
  tick=millis();
  output_set(LOW);
}

static void deactivate_output()
{
  dgk_active=false;
  output_set(LOW);
}

static void dgk_active_pattern(int pattern)
{
  if(pattern <DGK_MAX_PATTERN)
  {
    debug("DGK active pattern %d\n", pattern);
    activate_output(dgk_config.patterns[pattern].lowTime, dgk_config.patterns[pattern].highTime);
  }
  else
  {
    error("Invalid input\n");
  }
}

static bool config_range_check(int low, int high)
{
  return (low >= DGK_MIN_LOW &&
          low <= DGK_MAX_LOW &&
          high >= DGK_MIN_HIGH &&
          high <= DGK_MAX_HIGH);
}

static void dgk_config_pattern(int i, int low, int high)
{
  if(i>=DGK_MAX_PATTERN || !config_range_check(low, high))
  {
    error("Invalid input\n");
    return;
  }
  dgk_config.patterns[i].lowTime=low;
  dgk_config.patterns[i].highTime=high;
  dgk_config_save();
  debug("DGK saved config pattern: %d, low: %d, high: %d\n", i, low, high);
}

static void dgk_config_realscg(int low, int high)
{
  if(!config_range_check(low, high))
  {
    error("Invalid input\n");
    return;
  }
  dgk_config.realScg.lowTime=low;
  dgk_config.realScg.highTime=high;
  dgk_config_save();
  debug("DGK saved config real SCG low: %d, high: %d\n", low, high);
}

void dgk_process()
{
  if(dgk_active)
  {
    if(digitalRead(OUTPUT_PIN)==LOW)
    {
      if(millis()-tick>=highTime)
      {
        output_set(HIGH);
        tick=millis();
      }
    }
    else
    {
      if(millis()-tick>=lowTime)
      {
        output_set(LOW);
        tick=millis();
      }
    }
  }
}

void dgk_console_handle(char *result)
{
  if(__check_cmd("get config"))
  {
    dgk_show_config();
  }
  else if(__check_cmd("pattern "))
  {
    int i;
    if(sscanf(__param_pos("pattern "), "%d", &i)==1)
    {
      dgk_active_pattern(i);
    }
    else error("Invalid input\n");
  }
  else if(__check_cmd("config pattern "))
  {
    int i, low, high;
    if(sscanf(__param_pos("config pattern "), "%d %d %d", &i, &low, &high)==3)
    {
      dgk_config_pattern(i, low, high);
    }
    else error("Invalid input\n");
  }
  else if(__check_cmd("real scg"))
  {
    debug("DGK active real SCG\n");
    activate_output(dgk_config.realScg.lowTime, dgk_config.realScg.highTime);
  }
  else if(__check_cmd("config real scg "))
  {
    int low, high;
    if(sscanf(__param_pos("config real scg "), "%d %d", &low, &high)==2)
    {
      dgk_config_realscg(low, high);
    }
    else error("Invalid input\n");
  }
  else if(__check_cmd("stop"))
  {
    debug("DGK deactivated\n");
    deactivate_output();
  }
}