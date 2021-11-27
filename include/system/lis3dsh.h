#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#define LIS3DSH_ID 0x3f

typedef int (*lis3dsh_read_write)(void *out_data, size_t out_size, void *in_data, size_t in_size);

typedef struct {
    lis3dsh_read_write read_write;
} lis3dsh_callbacks;


int lis3dsh_init(lis3dsh_read_write read_write_cb);
int lis3dsh_get_id(uint8_t *id);
int lis3dsc_read_reg(uint8_t addr, uint8_t *content);
