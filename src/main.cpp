/* Includes requis pour esp32 avec wifi */
#include <WiFiManager.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
/* Constante pour la premiere connexion au reseau sans-fil */
const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";
/* Variable pour tester la pin 2 du esp32 (0-LOW 1-HIGH) */
int onoff = 0;
/* Variable pour le wifimanager qui permet de se connecter au esp32 */
WiFiManager wifiManager;
/* Fonction qui sexecute au demarrage du esp32 */ 
void setup() {
    /* Demarre le serial monitor du esp32 */ 
    Serial.begin(9600);
    /* Mode du wifi */
    WiFi.mode(WIFI_STA);
    /* Test un auto connexion au reseau a partir des informations envoyes */
    wifiManager.autoConnect(ssid,password);
    /* Mode de la pin output on envoie du courant */
    pinMode(2, OUTPUT);
}
void loop() {
    /* Variable qui va chercher l'adresse ip donne au esp32 */
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
    delay(5000);
    /* Envoie la variable sur la pin2 */
    digitalWrite(2, onoff);
    /* Alterne la valeur de la pin2 */
    if(onoff == 0)
    {
        onoff = 1;
    }
    else
    {
        onoff = 0;
    }
}