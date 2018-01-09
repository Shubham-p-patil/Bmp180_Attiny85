#include <oledDriver.h>
#include <Bmp180.h>
#include <TinyWireM.h>
#include <USI_TWI_Master.h>
bmpDriver sensor;
short temperature;
long pressure;
oledDriver oled;
void setup() {
  // put your setup code here, to run once:
    sensor.bmp180_init();
     oled.init();
     oled.clearScreen();
}

  // put your main code here, to run repeatedly:
void loop() {
 // first, read uncompensated temperature
 //temperature = bmp180ReadUT();
 //and then calculate calibrated temperature
 temperature = sensor.bmp180CorrectTemperature(sensor.bmp180ReadUT());
 // then , read uncompensated pressure
 //pressure = bmp180ReadUP();
 //and then calculate calibrated pressure
 pressure = sensor.bmp180CorrectPressure(sensor.bmp180ReadUP());
 
 oled.printNumber( pressure );
}

