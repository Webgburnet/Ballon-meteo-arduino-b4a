#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   Cet exemple d'esquisse illustre l'utilisation normale d'un objet TinyGPS ++ (TinyGPSPlus).
   Il nécessite l’utilisation de SoftwareSerial et suppose que vous avez un
   Un périphérique GPS série à 9600 bauds raccordé aux broches 2 (rx) et 3 (tx).
*/
static const int RXPin = A10, TXPin = A11;
static const uint32_t GPSBaud = 9600;

// L'objet TinyGPS ++
TinyGPSPlus gps;

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
int temperature;
int CO2PPM;

unsigned long timer1=0;
unsigned long timer2=0;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println("Une démonstration simple de TinyGPS ++ avec un module GPS connecté");
  Serial.print("Test de la bibliothèque TinyGPS ++ v. "); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  //CO2
  sensor.begin(9600);
    Serial.begin(115200);
    Serial.println("get a 'g', begin to read from sensor!");
    Serial.println("********************************************************");
    Serial.println();
}

void loop()
{
// Cette esquisse affiche des informations chaque fois qu'une nouvelle phrase est correctement codée.
  Serial.println("Time");
  if(millis()- timer1 > 64)
  {
    //Serial.println("Serial GPS");
    ss.listen();
    while (ss.available() > 0)
      if (gps.encode(ss.read()))
        displayInfo();
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println("Aucun GPS détecté: vérifiez le câblage.");
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
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print("  CO2: ");
        Serial.print(CO2PPM);
        Serial.println("");
    }
    timer2=millis();
  }
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
    temperature = (int)data[4] - 40;

    return true;
}
