#include "hardware/pio.h"
#include "spi.pio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pio_spi_inst {
    PIO pio;
    uint sm;
    uint cs_pin;
} pio_spi_inst_t;

void pio_spi_write8_blocking(const pio_spi_inst_t *spi, const uint8_t *src, size_t len);

void pio_spi_read8_blocking(const pio_spi_inst_t *spi, uint8_t *dst, size_t len);

void pio_spi_write8_read8_blocking(const pio_spi_inst_t *spi, uint8_t *src, uint8_t *dst, size_t len);

#ifdef __cplusplus
}
#endif