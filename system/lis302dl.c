#include "system/spi.h"
#include "system/dbgio.h"


#define LIS302DL_ID     0x3b

#define WHO_I_AM_REG    0x0f


static inline uint8_t make_command(bool is_read, bool is_multiple, uint8_t addr)
{
    return (is_read << 7) | (is_multiple << 6) | addr;
}


static uint8_t get_id(void)
{
    uint8_t id = 0;
    uint8_t cmd = make_command(true, false, WHO_I_AM_REG);

    spi_cs_enable(ACCELERO_SPI);

    if (spi_write_interrupt(ACCELERO_SPI, &cmd, sizeof(cmd))) {
        goto out;
    }

    if (spi_read_interrupt(ACCELERO_SPI, &id, sizeof(id))) {
        goto out;
    }

    spi_cs_disable(ACCELERO_SPI);

out:
    return id;
}


int lis302dl_init(void)
{
    int ret;

    ret = spi_init(ACCELERO_SPI);
    if (ret) {
        CONSOLE_ERROR("Can't init accelero SPI");
        return 1;
    }

    uint8_t id = get_id();
    if (id != LIS302DL_ID) {
        CONSOLE_ERROR("lis302dl id is 0x%x (must be 0x%x)", id, LIS302DL_ID);
        return 1;
    }

    return 0;
}

