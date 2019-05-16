#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TinyGPS++.h>
#include "SHT31.h"
#include "MutichannelGasSensor.h"
#include "seeed_bme680.h"
#include "rgb_lcd.h"

#define chipSelect 4 //Carte SD 
#define IIC_ADDR_BME  uint8_t(0x76)
#define led9 9
#define pin_dust 8
#define RX_CO2 A8
#define TX_CO2 A9
#define RX_GPS A10
#define TX_GPS A11

static const uint32_t GPSBaud = 9600;

// Objet
TinyGPSPlus gps; //GPS
SHT31 sht31 = SHT31(); //Capteur SHT31
Seeed_BME680 bme680(IIC_ADDR_BME);

// La connexion série au périphérique GPS
SoftwareSerial serial_GPS(RX_GPS, TX_GPS);

//C02
SoftwareSerial s_serial(RX_CO2, TX_CO2);      // TX, RX
#define sensor s_serial

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

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

unsigned long duration=0;
unsigned long savetime=0;
unsigned long sampletime_ms = 1000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

double latitude=0;
double longitude=0;
int annee=0;
int mois=0;
int jour=0;
int heure=0;
int minutes=0;
int seconde=0;
int milliseconde=0;

unsigned long timer1=0;
unsigned long timer2=0;
unsigned long timer3=0;

//Shield Ethernet sans Aoe Numero 2
int status = WL_IDLE_STATUS;
//char ssid[] = "MarcusEtire";         // Nom du réseau wifi
//char pass[] = "5D9D659735CE67DD367942A137";       // votre mot de passe réseau wifi(utilisez pour WPA ou comme clé pour WEP)
char ssid[] = "Sti-2k18-SIN";         // Nom du réseau wifi
char pass[] = "Sti-2k18-Sin";       // votre mot de passe réseau wifi(utilisez pour WPA ou comme clé pour WEP)
int keyIndex = 0;                 // votre numéro d'index de clé de réseau (nécessaire uniquement pour WEP)
unsigned int localPort = 5500;        // port local sur lequel écouter
IPAddress ip(192,168,2,123);

WiFiUDP Udp;

void setup()
{
  //Inistialisation du GPS
  Serial.begin(9600);
  Serial.println("Début Setup ");

  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  // Print a message to the LCD.
  lcd.print("IP:      ");
  delay(1000);

  //Initialisation Wifi
  Serial.println("Initialisation de la carte WiFi");
  WiFi.config(ip);
  Serial.print("Addresse IP statique : ");
  Serial.println(ip);
  //Essaie de se connecté sur Wifi
  do
  {
    Serial.print("En attente de connection sur le SSID : ");
    Serial.println(ssid);
    status = WiFi.begin(ssid,pass);
    lcd.setCursor(0, 1);
    lcd.print("Attente au Wifi  ");
    delay(10000);
  }
  while(status != WL_CONNECTED);
  // affiche le SSID du réseau auquel vous êtes connecté :
  Serial.print("SSID local: ");
  Serial.println(WiFi.SSID());
  //Adresse IP local
  Serial.print("Addresse IP local: ");
  Serial.println(WiFi.localIP());
  // affiche la longeur du signal reçu :
  long rssi = WiFi.RSSI();
  Serial.print("longeur du Signal (RSSI) : ");
  Serial.print(rssi);
  Serial.println(" dBm");
  //
  lcd.setCursor(0, 1);
  lcd.print("WiFi : OK         ");
  lcd.setCursor(3, 0);
  lcd.print(WiFi.localIP());
  delay(1000);
  
  //Initialisation du GPS
  Serial.print("Initialisation du GPS, version : ");
  serial_GPS.begin(GPSBaud);
  Serial.println(TinyGPSPlus::libraryVersion());
  lcd.setCursor(0, 1);
  lcd.print("GPS : OK       ");
  delay(1000);
  
  //CO2
  Serial.println("Initialisation capteur CO2 ");
  sensor.begin(9600);
  lcd.setCursor(0, 1);
  lcd.print("CO2 : OK       ");
  delay(1000);
  
  //SHT31 : Temperature + Humidite
  Serial.println("Initialisation capteur SHT31 ");
  sht31.begin();
  lcd.setCursor(0, 1);
  lcd.print("SHT31 : OK       ");
  delay(1000);
  

  //Capteur de CO
  Serial.print("Initialisation capteur Multigaz ");
  gas.begin(0x04);//Adresse I2C
  gas.powerOn();
  lcd.setCursor(0, 1);
  lcd.print("CO : OK       ");
  delay(1000);
   
  //BME680
  while (!bme680.init())
  {
    Serial.println("BME680 non branché");
    lcd.setCursor(0, 1);
    lcd.print("BME680 : PB      ");
  }  
  Serial.println("Initialisation capteur BME680");
  lcd.setCursor(0, 1);
  lcd.print("BME680 : OK      ");
  delay(1000);

  //Particules fines
  Serial.println("Initialisation capteur Particules Fines");
  pinMode(pin_dust,INPUT);
  lcd.setCursor(0, 1);
  lcd.print("Dust : OK       ");
  delay(1000);

  //Definition du port udp
   Udp.begin(localPort);
   lcd.setCursor(0, 1);
   lcd.print("UDP : OK           ");
   delay(1000);

    //Carte SD
   Serial.print("Initialisation de la carte SD ...");
     // voir si la carte SD est présente et peut être initialisée:
   while (!SD.begin(chipSelect)) 
  {
    Serial.println("Carte manquante ou non présente");
    lcd.setCursor(0, 1);
    lcd.print("INSERER CARTE SD    ");
  }
  Serial.println("carte initialisée !");
  lcd.setCursor(0, 1);
  lcd.print("SD : OK        ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Ballon Meteo        ");
  Serial.println("Fin Setup \n");
}

void loop()
{
  //Serial.println("Debut Loop");

     // Protocole Udp
  int Size=Udp.parsePacket();
  char message[Size];
  String conv_message_to_string;
  if(Size)
  {
    // lit le paquet dans packetBufffer
    Udp.read(message, Size);
    int cases = 0;
    while (cases != Size)
    {
      conv_message_to_string = conv_message_to_string + message[cases];
      cases = cases+1;
    }
    
    Serial.print("Paquet reçu de taille : ");
    Serial.println(Size);
    Serial.print("Adresse IP de  ");
    IPAddress remote = Udp.remoteIP();
    Serial.print(remote);
    Serial.print(", sur le port ");
    Serial.println(Udp.remotePort());
    Serial.print("Msg UDP:");
    Serial.println(conv_message_to_string);

    if(conv_message_to_string=="Acquer")
    {
      Serial.print("Le message est : ");
      Serial.print("Capteur actualisé");
      
      delay(1);
      
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write("CO2=");
      Udp.print(CO2PPM);
      Udp.write("ppm");
      Udp.write(" CO=");
      Udp.print((int)co);
      Udp.write("ppm");
      Udp.write("\n");
      Udp.write("Dust=");
      Udp.print((int)concentration);
      Udp.write(" P=");
      Udp.print((int)pression);
      Udp.write("hPa");
      Udp.write("\n");
      Udp.write("H=");
      Udp.print((int)hum);
      Udp.write("%RH");
      Udp.write(" T=");
      Udp.print(temp);
      Udp.write("C");
      Udp.write("\n");
//      Udp.write("P=");
//      Udp.print(pression);
//      Udp.write("Pa");
//      Udp.write("\n");
      Udp.endPacket();
    }
    else
    {
      char  ReplyBuffer[] = "acknowledged";
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
  }
  else
  {
  //Particules Fines
  if (digitalRead(pin_dust) == HIGH)
  {
    lowpulseoccupancy = lowpulseoccupancy+duration;
    savetime= micros();
    duration=0;
  }  
  while(digitalRead(pin_dust) == LOW)
  {
    duration=micros()-savetime;//+temps_en_cours;
  }
  
  //GPS
  if(millis()- timer1 > 64)
  {
    //Serial.println("Serial GPS");
    serial_GPS.listen();
    while (serial_GPS.available() > 0)
    {
      if (gps.encode(serial_GPS.read()))
      {
        displayInfo();
      }
    }
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println("Aucun GPS detecte: verifiez le cablage.");
      //while(true);
    }
    timer1=millis();
  }
  
  //CO2 + BME680 + SHT31 + +CO + Particules Fines
  if(millis()- timer2 > 1000)
  {
  //CO2
    sensor.listen();
    dataRecieve();
    
    //BME 680 : Pression
    bme680.read_sensor_data(); 
    pression = bme680.sensor_result_value.pressure/100;
    
    //SHT31 Temperature + Humidite
    temp = sht31.getTemperature();
    hum = sht31.getHumidity();
    
    //Multigaz : CO
    co = gas.measure_CO();
    
    //Particules Fines
    if(duration==0 && lowpulseoccupancy!=0)
    {
      ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    }
    
    //Affichage Valeurs sur le Moniteur Serie
    Serial.print("Pression = ");
    Serial.print(pression);
    Serial.println(" hPa");
    Serial.print("Temp = "); 
    Serial.print(temp);
    Serial.println(" C");
    Serial.print("Hum = "); 
    Serial.print(hum);
    Serial.println("%");
    Serial.print("CO2 : ");
    Serial.print(CO2PPM);
    Serial.println("ppm");
    Serial.print("CO = ");
    Serial.print(co);
    Serial.println(" ppm");
    Serial.print("duration : ");
    Serial.print(duration);
    Serial.println(" µs");
    Serial.print("LPO : ");
    Serial.println(lowpulseoccupancy);
    Serial.print("ratio : ");
    Serial.println(ratio);
    Serial.print("concentration : ");
    Serial.print(concentration);
    Serial.println(" pcs/L");
    
    //Rénitialisation des variables
    lowpulseoccupancy = 0;
    duration=0;
    timer2=millis();
  }

  // Carte SD
  // crée une chaîne pour assembler les données à consigner:
   String dataString = "";
   dataString += String(pression);
   dataString += ", ";
   dataString += String(temp);
   dataString += ", ";
   dataString += String(hum);
   dataString += ", ";
   dataString += String(CO2PPM);
   dataString += ", ";
   dataString += String(co);
   dataString += ", ";
   dataString += String(concentration);
   dataString += ", ";
   dataString += String(latitude,6);
   dataString += ", ";
   dataString += String(longitude,6);
   dataString += ", ";
   dataString += String(jour);
   dataString += "/";
   dataString += String(mois);
   dataString += "/";
   dataString += String(annee);
   dataString += ", ";
   dataString += String(heure);
   dataString += ":";
   dataString += String(minutes);
   dataString += ":";
   dataString += String(seconde);
   dataString += ", ";
   
    
  // ouvre le fichier. notez qu'un seul fichier peut être ouvert à la fois,
  // vous devez donc fermer celui-ci avant d'en ouvrir un autre.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // si le fichier est disponible, écrivez-le:
  if (dataFile) 
  {
    dataFile.println(dataString);
    dataFile.close();
//    Serial.println(dataString);
  }
  // si le fichier n'est pas ouvert, une erreur apparaît:
  else 
  {
    Serial.println("erreur d'ouverture datalog.txt");
  }

  //Serial.println("Fin Loop \n");
  }
}

void displayInfo()
{
  latitude=gps.location.lat();
  longitude=gps.location.lng();
  annee=gps.date.year();
  mois=gps.date.month();
  jour=gps.date.day();
  heure=gps.time.hour();
  minutes=gps.time.minute();
  seconde=gps.time.second();
  milliseconde=gps.time.centisecond();
  
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
    Serial.print(jour);
    Serial.print("/");
    Serial.print(mois);
    Serial.print("/");
    Serial.print(annee);
  }
  else
  {
    Serial.print("INVALIDE");
  }

  Serial.print(" ");
  if (gps.time.isValid())
  {
    if (heure < 10) Serial.print("0");
    Serial.print(heure);
    Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.print(minutes);
    Serial.print(":");
    if (seconde < 10) Serial.print("0");
    Serial.print(seconde);
    Serial.print(".");
    if (milliseconde < 10) Serial.print("0");
    Serial.print(milliseconde);
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

//    for(int j=0; j<9; j++)
//    {
//        Serial.print(data[j]);
//        Serial.print(" ");
//    }
//    Serial.println("");

    if((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8])
    {
        return false;
    }

    CO2PPM = (int)data[2] * 256 + (int)data[3];

    return true;
}
