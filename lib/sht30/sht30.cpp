#include "sht30.h"
#include "user_define.h"
#include "Arduino.h"
#include "Wire.h"

int sht_int(){
    //Wire.begin();
    Wire.beginTransmission(SHT30_I2C_ADDR);
    Wire.endTransmission();
    delay(100);
    return 0;
}

int8_t check_sht_i2c(){
    Wire.beginTransmission(SHT30_I2C_ADDR);
    int8_t error = Wire.endTransmission();
    return error;
}

int get_sht_data(sht30_t *sht){
    Wire.beginTransmission(SHT30_I2C_ADDR);
      // Send measurement command
    Wire.write(0x2C);
    Wire.write(0x06);
    // Stop I2C transmission
    Wire.endTransmission();
    //delay(100);
    // Request 6 bytes of data
    Wire.requestFrom(SHT30_I2C_ADDR, 6);
    if(Wire.available() == 6){
        sht->data[0] = Wire.read();
        sht->data[1] = Wire.read();
        sht->data[2] = Wire.read();
        sht->data[3] = Wire.read();
        sht->data[4] = Wire.read();
        sht->data[5] = Wire.read();
    }
    // Convert the data
    sht->ctemp = ((((sht->data[0] * 256.0) + sht->data[1]) * 175) / 65535.0) - 45;
    sht->ftemp = (sht->ctemp * 1.8) + 32;
    sht->humidity = ((((sht->data[3] * 256.0) + sht->data[4]) * 100) / 65535.0);
    return 0;
}
