#include <SoftwareSerial.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include "SHT31.h"
#include "MutichannelGasSensor.h"
#include "seeed_bme680.h"

/*
   Cet exemple d'esquisse illustre l'utilisation normale d'un objet TinyGPS ++ (TinyGPSPlus).
   Il nécessite l’utilisation de SoftwareSerial et suppose que vous avez un
   Un périphérique GPS série à 9600 bauds raccordé aux broches 2 (rx) et 3 (tx).
*/

#define IIC_ADDR  uint8_t(0x76)

static const int RXPin = A10, TXPin = A11;
static const uint32_t GPSBaud = 9600;

// Objet
TinyGPSPlus gps; //GPS
SHT31 sht31 = SHT31(); //Capteur SHT31
Seeed_BME680 bme680(IIC_ADDR);

// La connexion série au périphérique GPS
SoftwareSerial ss(RXPin, TXPin);

//C02
SoftwareSerial s_serial(A8, A9);      // TX, RX
#define sensor s_serial

const unsigned char cmd_get_sensor[] =
{
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};

unsigned char dataRevice[9];
int CO2PPM=0;

float temp = 0;
float hum = 0;

float co=0;
float pression = 0;

unsigned long timer1=0;
unsigned long timer2=0;
unsigned long timer3=0;

void setup()
{
  //Inistialisation du GPS
  Serial.begin(115200);
  Serial.println("Début Setup ");

  //Initialisation du GPS
  Serial.print("Initialisation du GPS, version : ");
  ss.begin(GPSBaud);
  
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  //CO2
  Serial.println("Initialisation capteur CO2 ");
  sensor.begin(9600);
  
  //SHT31 : Temperature + Humidite
  Serial.println("Initialisation capteur SHT31 ");
  sht31.begin();
  

  //Capteur de CO
  Serial.print("Initialisation capteur Multigaz, ");
  gas.begin(0x04);//Adresse I2C
  gas.powerOn();
   
  //BME680
  while (!bme680.init()) 
    Serial.println("Branché le BME680");
  Serial.println("Initialisation capteur BME680");

  Serial.println("Fin Setup \n");
}

void loop()
{
  Serial.println("Debut Loop");
  
  //GPS
  if(millis()- timer1 > 64)
  {
    //Serial.println("Serial GPS");
    ss.listen();
    while (ss.available() > 0)
      if (gps.encode(ss.read()))
        displayInfo();
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println("Aucun GPS detecte: verifiez le cablage.");
      //while(true);
    }
    timer1=millis();
  }
  
  //CO2
  if(millis()- timer2 > 128)
  {
    //Serial.println("Serial CO2");
  sensor.listen();
  if(dataRecieve())
    {
        Serial.print("CO2 : ");
        Serial.println(CO2PPM);
    }
    timer2=millis();
  }

  //SHT31 Temperature + Humidite
  temp = sht31.getTemperature();
  hum = sht31.getHumidity();
  Serial.print("Temp = "); 
  Serial.print(temp);
  Serial.println(" C"); //The unit for  Celsius because original arduino don't support speical symbols
  Serial.print("Hum = "); 
  Serial.print(hum);
  Serial.println("%"); 
  
  //Multigaz : CO
  co = gas.measure_CO();
  Serial.print("CO = ");
  if(co>=0) 
  {
    Serial.print(co);
    Serial.println(" ppm");
  }
  else
  { 
    Serial.println("invalid");
    
  }

  //BME 680 : Pression
  if(millis()- timer3 > 192)
  {
    bme680.read_sensor_data(); 
    pression = bme680.sensor_result_value.pressure;
    Serial.print("Pression = ");
    Serial.print(pression);
    Serial.println(" Pa");
    timer3=millis();
  }

  //Particules Fines

  
  Serial.println("Fin Loop \n");
}

void displayInfo()
{
  double latitude=gps.location.lat();
  double longitude=gps.location.lng();
  Serial.print("Emplacement : "); 
  if (gps.location.isValid())
  {
    Serial.print(latitude, 6);
    Serial.print(",");
    Serial.print(longitude, 6);
  }
  else
  {
    Serial.print("INVALIDE");
  }

  Serial.print("  Date/Heure: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print("INVALIDE");
  }

  Serial.print(" ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print("0");
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print("0");
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print("0");
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print("0");
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print("INVALIDE");
  }

  Serial.println();
}

//CO2
bool dataRecieve(void)
{
    byte data[9];
    int i = 0;

    //transmit command data
    for(i=0; i<sizeof(cmd_get_sensor); i++)
    {
        sensor.write(cmd_get_sensor[i]);
    }
    delay(10);
    //begin reveiceing data
    if(sensor.available())
    {
        while(sensor.available())
        {
            for(int i=0;i<9; i++)
            {
                data[i] = sensor.read();
            }
        }
    }

    for(int j=0; j<9; j++)
    {
        Serial.print(data[j]);
        Serial.print(" ");
    }
    Serial.println("");

    if((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8])
    {
        return false;
    }

    CO2PPM = (int)data[2] * 256 + (int)data[3];

    return true;
}
