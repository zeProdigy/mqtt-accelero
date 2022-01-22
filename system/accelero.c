#include <math.h>

#include "drivers/lis3dsh.h"
#include "system/accelero.h"
#include "system/dbgio.h"
#include "system/spi.h"


static int read_write(void *out_data, size_t out_size, void *in_data, size_t in_size)
{
    int ret;

    spi_cs_enable(ACCELERO_SPI);
    ret = spi_exchange_polling(ACCELERO_SPI, out_data, out_size, in_data, in_size);
    spi_cs_disable(ACCELERO_SPI);

    return ret;
}


int accelero_init(void)
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


int accelero_get_xyz(accelero_xyz_t *out)
{
    lis3dsh_xyz_t data;

    if (lis3dsh_get_xyz(&data)) {
        CONSOLE_ERROR("Read accelero data error");
        return 1;
    }

    out->x = data.x;
    out->y = data.y;
    out->z = data.z;

    return 0;
}


int accelero_get_tilt(accelero_tilt_t *out)
{
    lis3dsh_xyz_t data;

    if (lis3dsh_get_xyz(&data)) {
        CONSOLE_ERROR("Read accelero data error");
        return 1;
    }

    float mul = 180 / M_PI;

    out->pitch = atan2(-data.x, sqrt(data.y * data.y + data.z * data.z)) * mul;
	out->roll  = atan2(data.y, sqrt(data.x * data.x + data.z * data.z)) * mul;

    return 0;
}
