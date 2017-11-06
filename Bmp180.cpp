#include "Bmp180.h"
#include <TinyWireM.h>


///Please refer to page no 15 of Datasheet for reference////

/*  Every sensor module has individual coefficients. Before the first calculation of
temperature and pressure, the master reads out the E2PROM data*/


///Read from EEPROM address for getting initial values for future calculation of temperature and values
int bmpDriver::bmp180ReadCaliberation(unsigned char EEPROM_address){

  unsigned char msb, lsb;
  TinyWireM.beginTransmission(SlaveAddress);
  TinyWireM.send(EEPROM_address);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(SlaveAddress, 2);
  while(TinyWireM.available()<2);
  msb = TinyWireM.receive();
  lsb = TinyWireM.receive();
  return (int) msb<<8 | lsb;    ///Making an 16 bit value again
}
void bmpDriver::bmp180_init() {

  TinyWireM.begin();
  // First read calibration data from EEPROM
  ac1 = bmp180ReadCaliberation(0xAA);
  ac2 = bmp180ReadCaliberation(0xAC);
  ac3 = bmp180ReadCaliberation(0xAE);
  ac4 = bmp180ReadCaliberation(0xB0);
  ac5 = bmp180ReadCaliberation(0xB2);
  ac6 = bmp180ReadCaliberation(0xB4);
  b1 = bmp180ReadCaliberation(0xB6);
  b2 = bmp180ReadCaliberation(0xB8);
  mb = bmp180ReadCaliberation(0xBA);
  mc = bmp180ReadCaliberation(0xBC);
  md = bmp180ReadCaliberation(0xBE);

}
///Get the uncompensatedTemperature from bmp180
unsigned int bmpDriver::bmp180ReadUT(){

  unsigned int uncompensatedTemperature;

  // Write 0x2E into Register 0xF4 and wait at least 4.5mS
  // This requests a temperature reading
  // with results in 0xF6 and 0xF7
  TinyWireM.beginTransmission(SlaveAddress);
  TinyWireM.send(0xF4);
  TinyWireM.send(0x2E);
  TinyWireM.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Then read two bytes from registers 0xF6 (MSB) and 0xF7 (LSB)
  // and combine as unsigned integer
  uncompensatedTemperature = bmp180ReadCaliberation(0xF6);
  return uncompensatedTemperature;
}

///Correct the Temperature values and return corrected value
double bmpDriver::bmp180CorrectTemperature(unsigned int uncompensatedTemperature){

  x1 = (((long)uncompensatedTemperature - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;
  return (((b5 + 8)>>4));
}

///Get the uncompensatedPressure from bmp180
unsigned long bmpDriver::bmp180ReadUP(){

  unsigned char msb, lsb, xlsb;
  unsigned long uncompensatedPressure = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  TinyWireM.beginTransmission(SlaveAddress);
  TinyWireM.send(0xF4);
  TinyWireM.send(0x34 + (OSS<<6));
  TinyWireM.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(5 + (5*OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  TinyWireM.beginTransmission(SlaveAddress);
  TinyWireM.send(0xF6);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(SlaveAddress, 3);

  // Wait for data to become available
  while(TinyWireM.available() < 3)
  ;
  msb = TinyWireM.receive();
  lsb = TinyWireM.receive();
  xlsb = TinyWireM.receive();

  uncompensatedPressure = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return uncompensatedPressure;
}

///Correct the Pressure values and return corrected value
double bmpDriver::bmp180CorrectPressure(unsigned long uncompensatedPressure){


  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(uncompensatedPressure - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    correctedPressure = (b7<<1)/b4;
  else
  correctedPressure = (b7/b4)<<1;
  x1 = (correctedPressure>>8) * (correctedPressure>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * correctedPressure)>>16;
  correctedPressure += (x1 + x2 + 3791)>>4;

  return correctedPressure;
}
