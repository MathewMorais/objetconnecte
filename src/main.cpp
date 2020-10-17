/* Include requis pour faire fonctionner le relais */
#include <Arduino.h>
/* Variable exemple qui determine si le relai est ouvert ou fermer en mode NC */
bool condition = false;
/* Execute au demarrage */
void setup() {
    /* Demarre le serial monitor */ 
    Serial.begin(9600);
    /* Definie la pin 34 (branche au relais) comme output donc envoie du courant */ 
    pinMode(34, OUTPUT);
    digitalWrite(34, 0);
}
void loop() {
    /* Si la condition est rempli on met la pin 34 a high donc le relais souvre */
    if(condition)
    {
        digitalWrite(34, 1);
    }
    else
    {
        digitalWrite(34, 0);
    }
    delay(2000);
}