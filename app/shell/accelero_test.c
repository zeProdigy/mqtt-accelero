#include <string.h>

#include "system/dbgio.h"
#include "system/spi.h"
#include "system/lis3dsh.h"


static int read_write(void *out_data, size_t out_size, void *in_data, size_t in_size)
{
    int ret;

    spi_cs_enable(ACCELERO_SPI);
    ret = spi_exchange_polling(ACCELERO_SPI, out_data, out_size, in_data, in_size);
    spi_cs_disable(ACCELERO_SPI);

    return ret;
}


static int init(void)
{
    int ret;

    ret = spi_init(ACCELERO_SPI);
    if (ret) {
        CONSOLE_ERROR("Can't init accelero SPI");
        return 1;
    }

    ret = lis3dsh_init(read_write);
    if (ret) {
        CONSOLE_ERROR("Can't init lis3dsh");
        return 1;
    }

    uint8_t id;
    ret = lis3dsh_get_id(&id);
    if (ret) {
        CONSOLE_ERROR("Can't get accelero id");
    }

    if (id != LIS3DSH_ID) {
        CONSOLE_ERROR("lis3dsh id is 0x%x (must be 0x%x)", id, LIS3DSH_ID);
        return 1;
    }

    CONSOLE_LOG("lis3dsh inited");

    return 0;
}


int accelero_test(int argc, char *argv[])
{
    if (argc < 2) {
        CONSOLE_LOG("No command");
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "init") == 0) {
        init();
    } else if (strcmp(cmd, "read") == 0) {
        if (argc < 3) {
            CONSOLE_LOG("Need to specify reg addr");
            return 1;
        }
        uint8_t addr = strtol(argv[2], NULL, 16);
        uint8_t content;
        lis3dsh_read_reg(addr, &content);
        CONSOLE_LOG("reg 0x%x: 0x%x", addr, content);
    } else if (strcmp(cmd, "acc") == 0) {
        lis3dsh_acc_t data;
        while(true) {
            if (lis3dsh_get_acc(&data)) {
                CONSOLE_ERROR("Read data error");
                return 1;
            }
            CONSOLE_LOG("x: %f y: %f z: %f", data.acc_x, data.acc_y, data.acc_z);
        }
    } else if (strcmp(cmd, "tilt") == 0) {
        lis3dsh_tilt_t data;
        while(true) {
            if (lis3dsh_get_tilt(&data)) {
                CONSOLE_ERROR("Read data error");
                return 1;
            }
            CONSOLE_LOG("pitch: %f roll: %f", data.pitch, data.roll);
        }
    } else {
        CONSOLE_LOG("Unsupported cmd");
    }

    return 0;
}
