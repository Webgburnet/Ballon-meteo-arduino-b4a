#define pin_particule_fine 8

unsigned long duration;
unsigned long temps;
unsigned long cste_temps_ms = 30000;
unsigned long occupation_pulse = 0;
float ratio = 0;
float concentration = 0;

void setup() 
{
    Serial.begin(9600);
    Serial.println("Début setup");
    pinMode(pin_particule_fine,INPUT);
    temps = millis();
    Serial.println("Fin setup");
}

void loop() 
{
    duration = pulseIn(pin_particule_fine, LOW);
    occupation_pulse = occupation_pulse+duration;

    if ((millis()-temps) > cste_temps_ms)
    {
        ratio = occupation_pulse/(cste_temps_ms*10.0);  // Pourcentage entier 0 => 100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // utilisation de la courbe de fiche technique
        Serial.print(occupation_pulse);
        Serial.print(",");
        Serial.print(ratio);
        Serial.print(",");
        Serial.println(concentration);
        occupation_pulse = 0;
        temps = millis();
    }
}
