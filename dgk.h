#include <stdint.h>

#define DGK_MAX_PATTERN 16

#define DEFAULT_PATTERN_LOW 1000
#define DEFAULT_PATTERN_HIGH 1000
#define DEFAULT_REALSCG_LOW 5000
#define DEFAULT_REALSCG_HIGH 5000

#define DGK_MIN_LOW 100
#define DGK_MAX_LOW 65000
#define DGK_MIN_HIGH 100
#define DGK_MAX_HIGH 65000

typedef struct
{
  uint16_t lowTime;
  uint16_t highTime;
}dgk_config_t;

typedef struct
{
  dgk_config_t patterns[DGK_MAX_PATTERN];
  dgk_config_t realScg;
  uint16_t crc;
}dgk_all_config_t;

void dgk_init();
void dgk_process();
void dgk_console_handle(char *result);
