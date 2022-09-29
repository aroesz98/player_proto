#include <stdint.h>
#include "pico/stdlib.h"

#define SD_MOSI 15
#define SD_MISO 12
#define SD_CLK 14
#define SD_CS 26

#ifdef __cplusplus
extern "C" {
#endif

static inline void sd_spi_chip_select(uint8_t level)
{
    gpio_put(SD_CS, level);
}

#ifdef __cplusplus
}
#endif
