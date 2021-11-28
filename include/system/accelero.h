#pragma once


typedef struct {
    float x;
    float y;
    float z;
} accelero_xyz_t;

typedef struct {
    float roll;
    float pitch;
} accelero_tilt_t;


int accelero_init(void);
int accelero_get_xyz(accelero_xyz_t *out);
int accelero_get_tilt(accelero_tilt_t *out);
