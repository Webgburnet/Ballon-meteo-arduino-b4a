/* 
* PROJET DOMOTIQUE STI2D SIN
* 
* Arduino Uno
* 
* A0: NC
* A1: NC
* A2: NC
* A3: NC
* A4: NC
* A5: NC
* 
* pin0 : NC
* pin1 : NC
* pin2 : NC
* pin3 : NC
* 
* pin4 : NC
* pin5 : NC
* pin6 : NC
* pin7 : NC
* pin8 : NC
* 
* pin9 : NC
* pin10 : NC
* pin11 : NC
* pin12 : NC
* pin13 : NC 
* 
* I2C : NC
* I2C : NC
* 
* Shield Wifi
* 
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>

//Shield Ethernet sans Aoe Numero 2
int status = WL_IDLE_STATUS;
char ssid[] = "MarcusEtire";         // Nom du réseau wifi
char pass[] = "5D9D659735CE67DD367942A137";       // votre mot de passe réseau wifi(utilisez pour WPA ou comme clé pour WEP)
int keyIndex = 0;                 // votre numéro d'index de clé de réseau (nécessaire uniquement pour WEP)
unsigned int localPort = 5500;        // port local sur lequel écouter

WiFiUDP Udp;

void setup() {
  Serial.begin(9600);
  Serial.println("Début Setup");
  status = WiFi.begin(ssid,pass);
  printWifiStatus();
  Udp.begin(localPort);
  Serial.println("Fin Setup");
}

void loop() {
  // Protocole Udp
  int Size=Udp.parsePacket();
  char message[Size];
  String conv_message_to_string;
  //Variable Capteur temperature et humidité
  float capteur1 = 10.10;
  int capteur2 = 20;
  
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
    Serial.println("Msg UDP:");
    Serial.println(conv_message_to_string);

    if(conv_message_to_string=="Acquer")
    {
      Serial.print("Le message est : ");
      Serial.print("Capteur actualisé");
      
      delay(1);
      
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      
      Udp.write("Capteur 1 = ");
      Udp.print(capteur1);
      Udp.write(" unite");
      Udp.write("\n");
      Udp.write("Pression = ");
      Udp.print(capteur2);
      Udp.write(" unite");
      Udp.write("\n");
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

  /*Liste instruction pour B4A :
   * Arrete : Arreter le store
   */

   if(conv_message_to_string=="Arrete")
   {
    Serial.println("Action de l'arduino");
   }
}

void printWifiStatus() {
  // affiche le SSID du réseau auquel vous êtes connecté :
  Serial.print("SSID : ");
  Serial.println(WiFi.SSID());

  // affiche l'adresse IP du Shield WiFi :
  IPAddress ip = WiFi.localIP();
  Serial.print("Addresse IP : ");
  Serial.println(ip);

  // affiche la longeur du signal reçu :
  long rssi = WiFi.RSSI();
  Serial.print("longeur du Signal (RSSI) : ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
