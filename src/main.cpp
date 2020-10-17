/* Liste des includes pour le oled */
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
/* Using pour retirer les std:: */
using namespace std;
/* Constructeur pour la variable du oled */
Adafruit_SSD1306 display(128, 64, &Wire, -1);
/* Variables pour afficher sur le oled */
int tempcible = 25;
String filtrecadence = "30 minutes";
/* Fonction pour afficher sur le oled */
void displayoled()
{
    display.clearDisplay();
    /* Ligne 1 */ 
    display.setCursor(0, 0);
    display.println("Temp. actue : 12");
    /* Ligne 2 */ 
    display.setCursor(0, 20);
    char buffer [33];
    String str = String(itoa(tempcible,buffer,10));
    display.println("Temp. cible : "+str);
    /* Ligne 3 */ 
    display.setCursor(0, 35);
    display.print("Filtre : "+filtrecadence);
    /* Ligne 4 */ 
    display.setCursor(0, 50);
    String ip = "192.168.2.35";
    display.print("IP : ");
    display.print(ip);
    /* Fonction pour actionner l'affichage */
    display.display();
}
/* Fonction qui s'active une fois au demarrage du esp32 */
void setup() {
    /* Verifie que le i2c est bien connecter au oled */
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    delay(2000);
    /* Definie la grosseur du texte sur le oled */
    display.setTextSize(1);
    displayoled();
}
void loop() {

}