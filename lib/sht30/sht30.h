#ifndef SHT30_H
#define SHT30_H

#include "user_define.h"

typedef struct{
    unsigned int data[6];
    float ctemp;
    float ftemp;
    float humidity;
}sht30_t;

int sct_int();
int get_sct_data(sht30_t *sht);


#endif