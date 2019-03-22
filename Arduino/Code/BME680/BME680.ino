#include "seeed_bme680.h"
#define IIC_ADDR  uint8_t(0x76)


/**  NOTE!!!!!!!!!!!!  Select the communication protocol correctly **/

Seeed_BME680 bme680(IIC_ADDR); /* IIC PROTOCOL */

float temperature_BME680=0;
float pression_BME680=0;
float humidite_BME680=0;
float gaz_BME680=0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Serial start!!!");
  delay(100);
  while (!bme680.init()) 
  {
    Serial.println("bme680 init failed ! can't find device!");
  delay(10000);
  }
}

void loop() {
  if (bme680.read_sensor_data()) 
  {
    Serial.println("Failed to perform reading :(");
    return;
  }
  
  temperature_BME680 = bme680.sensor_result_value.temperature;
  Serial.print("temperature ===>> ");
  Serial.print(temperature_BME680);
  Serial.println(" C");
  
  pression_BME680 = bme680.sensor_result_value.pressure/ 1000.0;
  Serial.print("pression ===>> ");
  Serial.print(pression_BME680);
  Serial.println(" KPa");
  
  humidite_BME680 = bme680.sensor_result_value.humidity;
  Serial.print("humidité ===>> ");
  Serial.print(humidite_BME680);
  Serial.println(" % RH");
  
  gaz_BME680 = bme680.sensor_result_value.gas/ 1000.0;
  Serial.print("gaz ===>> ");
  Serial.print(gaz_BME680);
  Serial.println(" Kohms");

  Serial.println();
  Serial.println();
  
  delay(2000);
}
