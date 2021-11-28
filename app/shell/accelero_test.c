#include <string.h>

#include "system/dbgio.h"
#include "system/spi.h"
#include "system/accelero.h"
#include "drivers/lis3dsh.h"


int accelero_test(int argc, char *argv[])
{
    if (argc < 2) {
        CONSOLE_LOG("No command");
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "init") == 0) {
        accelero_init();
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
        accelero_xyz_t data;
        while(true) {
            if (accelero_get_xyz(&data)) {
                return 1;
            }
            CONSOLE_LOG("x: %f y: %f z: %f", data.x, data.y, data.z);
        }
    } else if (strcmp(cmd, "tilt") == 0) {
        accelero_tilt_t data;
        while(true) {
            if (accelero_get_tilt(&data)) {
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
