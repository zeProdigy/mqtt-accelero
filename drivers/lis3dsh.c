#include <assert.h>
#include <math.h>

#include "lis3dsh.h"


#define WHO_I_AM_REG    0x0f
#define STATUS_REG      0x27

#define CONTROL_4_REG   0x20
#define CONTROL_1_REG   0x21
#define CONTROL_2_REG   0x22
#define CONTROL_3_REG   0x23
#define CONTROL_5_REG   0x24
#define CONTROL_6_REG   0x25

#define OUT_X_L_REG     0x28
#define OUT_X_H_REG     0x29
#define OUT_Y_L_REG     0x2A
#define OUT_Y_H_REG     0x2B
#define OUT_Z_L_REG     0x2C
#define OUT_Z_H_REG     0x2D


// Можно было бы таскать коллбэки в контексте и передавать в функции каждый раз, но 
// мне кажется, нет смысла инстанцировать несколько "акселерометров" с разными контекстами
static struct {
    lis3dsh_read_write read_write;
    float scale;
} _ctx;


static inline uint8_t make_command(bool is_read, uint8_t addr)
{
    return (is_read << 7) | addr;
}


static inline int read_write(void *out_data, size_t out_size, void *in_data, size_t in_size)
{
    assert(_ctx.read_write);
    return _ctx.read_write(out_data, out_size, in_data, in_size);
}


int lis3dsh_init(lis3dsh_read_write read_write_cb)
{
    assert(read_write_cb);

    _ctx.read_write = read_write_cb;
    _ctx.scale = 2000.0;

    uint8_t cmd[2];
    cmd[0] = make_command(false, CONTROL_4_REG);
    cmd[1] = 0x67;

    read_write(&cmd, sizeof(cmd), NULL, 0);

    return 0;
}


int lis3dsh_get_id(uint8_t *id)
{
    uint8_t cmd = make_command(true, WHO_I_AM_REG);
    *id = 0;
    return read_write(&cmd, sizeof(cmd), id, sizeof(id));
}


int lis3dsh_read_reg(uint8_t addr, uint8_t *content)
{
    uint8_t cmd = make_command(true, addr);
    *content = 0;
    return read_write(&cmd, sizeof(cmd), content, sizeof(*content));
}


int lis3dsh_get_xyz(lis3dsh_xyz_t *out)
{
    uint8_t buf[6];
    uint8_t cmd = make_command(true, OUT_X_L_REG);

    if (read_write(&cmd, sizeof(cmd), buf, sizeof(buf))) {
        return 1;
    }

    int16_t x, y, z;
    x = (buf[1] << 8) | buf[0];
	y = (buf[3] << 8) | buf[2];
	z = (buf[5] << 8) | buf[4];

    out->x = x / _ctx.scale;
    out->y = y / _ctx.scale;
    out->z = z / _ctx.scale;

    return 0;
}
