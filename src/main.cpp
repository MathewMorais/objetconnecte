/* Liste des includes pour le senseur */ 
#include <OneWire.h>
#include <DallasTemperature.h>

/* Definie la pin ou le senseur est connecte */
#define ONE_WIRE_BUS 34

/* Definie la variable pour gerer le onewire qui permet de lire les datas a partir d'un pullup (resistance 4.7k)*/
OneWire oneWire(ONE_WIRE_BUS);

/* Variable de gestion des senseurs a partir du onewire */
DallasTemperature sensors(&oneWire);

/* Adresse du senseur connecte (1 variable pour 1 senseur)*/
DeviceAddress insideThermometer;
/* Fonction qui sexecute au demarrage du esp32 */
void setup() {
    Serial.begin(9600);
    sensors.begin();
    delay(2000);
}
void loop() {
    /* Variable qui cherche la temperature en celsius du senseur */
    float tempC = sensors.getTempC(insideThermometer);
    Serial.print(tempC);
    delay(2000);
}