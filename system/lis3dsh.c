#include <assert.h>

#include "system/lis3dsh.h"
#include "system/dbgio.h"


#define WHO_I_AM_REG    0x0f
#define STATUS_REG      0x27

#define CONTROL_4_REG   0x20
#define CONTROL_1_REG   0x21
#define CONTROL_2_REG   0x22
#define CONTROL_3_REG   0x23
#define CONTROL_5_REG   0x24
#define CONTROL_6_REG   0x25


// Можно было бы таскать коллбэки в контексте и передавать в функции каждый раз, но 
// мне кажется, нет смысла инстанцировать несколько "аккселерометров" с разными контекстами
static lis3dsh_callbacks _ctx;


static inline uint8_t make_command(bool is_read, bool is_multiple, uint8_t addr)
{
    return (is_read << 7) | (is_multiple << 6) | addr;
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

    uint8_t cmd[2];
    cmd[0] = make_command(false, false, CONTROL_4_REG);
    cmd[1] = 0x67;

    read_write(&cmd, sizeof(cmd), NULL, 0);

    return 0;
}


int lis3dsh_get_id(uint8_t *id)
{
    uint8_t cmd = make_command(true, false, WHO_I_AM_REG);
    *id = 0;
    return read_write(&cmd, sizeof(cmd), id, sizeof(id));
}


int lis3dsc_read_reg(uint8_t addr, uint8_t *content)
{
    uint8_t cmd = make_command(true, false, addr);
    *content = 0;
    return read_write(&cmd, sizeof(cmd), content, sizeof(*content));
}
