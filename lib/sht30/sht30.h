#ifndef SHT30_H
#define SHT30_H

#include "user_define.h"

typedef struct{
    unsigned int data[6];
    float ctemp;
    float ftemp;
    float humidity;
}sht30_t;

int sht_int();
int get_sht_data(sht30_t *sht);
int8_t check_sht_i2c();

#endif