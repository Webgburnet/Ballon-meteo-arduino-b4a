#include <Wire.h>
#include "MutichannelGasSensor.h"

void setup()
{
    Serial.begin(9600);
    Serial.println("Début Setup");
    gas.begin(0x04);// l'adresse I2C par défaut de l'esclave est 0x04
    gas.powerOn();
    Serial.print("Version du Firmware = ");
    Serial.println(gas.getVersion());
    Serial.println("Fin Setup");
}

void loop()
{
    float c;

    c = gas.measure_NH3();
    Serial.print("La concentration de NH3 : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_CO();
    Serial.print("La concentration de CO : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_NO2();
    Serial.print("La concentration de NO2 : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_C3H8();
    Serial.print("La concentration de C3H8 : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_C4H10();
    Serial.print("La concentration de C4H10 : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_CH4();
    Serial.print("La concentration de CH4 : ");
    Serial.print(c);
    else Serial.print("invalid");
    Serial.println(" ppm");

    c = gas.measure_H2();
    Serial.print("La concentration de H2 : ");
    Serial.print(c);
    Serial.println(" ppm");

    c = gas.measure_C2H5OH();
    Serial.print("La concentration de C2H5OH : ");
    Serial.print(c);
    Serial.println(" ppm");

    delay(1000);
    Serial.println("...");
}
