#include <SoftwareSerial.h>
SoftwareSerial s_serial(2, 3);      // TX, RX

#define capteur_co2 s_serial

const unsigned char cmd_capteur[] =
{
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};

unsigned char donneeRecu[9];
int temperature;
int CO2PPM;

void setup()
{
    capteur_co2.begin(9600);
    Serial.begin(9600);
    Serial.println("Obtenez un 'g', commencez à lire à partir de capteur_co2!");
    Serial.println("********************************************************");
    Serial.println();
}

void loop()
{
    if(donnee_recu())
    {
        Serial.print("Temperature: ");
        Serial.println(temperature);
        Serial.print("CO2: ");
        Serial.println(CO2PPM);
    }
    delay(1000);
}

bool donnee_recu(void)
{
    byte donnee[9];
    int i = 0;

    // transmettre la commande donnee
    for(i=0; i<sizeof(cmd_capteur); i++)
    {
        capteur_co2.write(cmd_capteur[i]);
    }
    delay(10);
    // commence à revoir les données
    if(capteur_co2.available())
    {
        while(capteur_co2.available())
        {
            for(int i=0;i<9; i++)
            {
                donnee[i] = capteur_co2.read();
            }
        }
    }

    for(int j=0; j<9; j++)
    {
        Serial.print(donnee[j]);
        Serial.print(" ");
    }
    Serial.println("");

    if((i != 9) || (1 + (0xFF ^ (byte)(donnee[1] + donnee[2] + donnee[3] + donnee[4] + donnee[5] + donnee[6] + donnee[7]))) != donnee[8])
    {
        return false;
    }

    CO2PPM = (int)donnee[2] * 256 + (int)donnee[3];
    temperature = (int)donnee[4] - 40;

    return true;
}
