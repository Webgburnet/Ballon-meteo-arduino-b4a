#include <SoftwareSerial.h>
SoftwareSerial GPS_Serial(2, 3);
unsigned char buffer[64];                   // tableau tampon pour la réception des données sur le port série
int compteur=0;                             // compteur pour tableau tampon
void setup()
{   
    Serial.begin(9600);  
    Serial.println("Début setup");
    GPS_Serial.begin(9600);
    Serial.println("Fin setup");
}

void loop()
{
    if (GPS_Serial.available())                     // si la date provient du port série du logiciel ==> les données proviennent du bouclier GPS_Serial
    {
        while(GPS_Serial.available())               // lecture de données dans un tableau de caractères
        {
            buffer[compteur++]=GPS_Serial.read();      // écrit des données dans un tableau
            if(compteur == 64)break;
        }
        Serial.write(buffer,compteur);                 // si aucune transmission de données ne se termine, écrivez un tampon sur le port série du matériel
        Effacer_tableau_tampon();                         // appelez la fonction Effacer_tableau_tampon pour effacer les données stockées du tableau
        compteur = 0;                                  // met le compteur de la boucle while à zéro
    }
    if (Serial.available())                 // si les données sont disponibles sur le port série matériel ==> les données proviennent d'un PC ou d'un ordinateur portable
    GPS_Serial.write(Serial.read());        // l'écrit dans le bouclier GPS_Serial
}


void Effacer_tableau_tampon()                     // fonction pour effacer le tableau tampon
{
    for (int i=0; i<compteur;i++)
    {
        buffer[i]=NULL;
    }                     // efface tout l'index du tableau avec la commande NULL
}
