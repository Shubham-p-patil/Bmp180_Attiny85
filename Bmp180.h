#ifndef Bmp180_h
#define Bmp180_h

#include <Arduino.h>



#define SlaveAddress 0x77
//#define OSS  0  // Oversampling Setting

/*Page 12 Datasheet
OSS=0 ultra Low Power Setting, 1 sample, 4.5 ms 3uA
OSS=1 Standard Power Setting, 2 samples, 7.5 ms 5uA
OSS=2 High Resolution,              4 samples, 13.5 ms 7uA
OSS=3 Ultra High Resolution,    2 samples, 25.5 ms 12uA
*/
class bmpDriver {

private:
    int ac1;
    int ac2;
    int ac3;

    unsigned int ac4;
    unsigned int ac5;
    unsigned int ac6;

    int b1;
    int b2;
    int mb;
    int mc;
    int md;
    long b5;

    //define variables for pressure and temperature calculation
    long x1,x2;

    //define variables for pressure calculation
    long x3,b3,b6,correctedPressure;
    unsigned long b4,b7;

    const unsigned char OSS = 0;  // Oversampling Setting

public:

    void bmp180_init();

    int bmp180ReadCaliberation(unsigned char EEPROM_address);

    unsigned int bmp180ReadUT();

    double bmp180CorrectTemperature(unsigned int uncompensatedTemperature);

    unsigned long bmp180ReadUP();

    double bmp180CorrectPressure(unsigned long uncompensatedPressure);


};
#endif
