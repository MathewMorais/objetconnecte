/* Liste des includes pour l'oled */
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>
/* using pour retirer les std:: */
using namespace std;
/* Constructeur du display pour le oled */
Adafruit_SSD1306 display(128, 64, &Wire, -1);
/* Variables utiliser sur le oled */
int tempcible = 25;
boolean refresholed = true;
String filtrecadence = "30 minutes";
/* Fonction pour afficher les informations sur le oled */
void displayoled()
{
  display.clearDisplay();
  /* Ligne 1 du display */
  display.setCursor(0, 0);
  display.println("Temp. actue : 12");
  /* Ligne 2 du display */
  display.setCursor(0, 20);
  char buffer [33];
  String str = String(itoa(tempcible,buffer,10));
  display.println("Temp. cible : "+str);
  /* Ligne 3 du display */
  display.setCursor(0, 35);
  display.print("Filtre : "+filtrecadence);
  /* Ligne 4 du display */
  display.setCursor(0, 50);
  String ip = "192.168.2.35";
  display.print("IP : ");
  display.print(ip);
  /* Commande pour actionner l'affichage */
  display.display();
}
/* Setup du programme se lance au demarrage du esp32 */
void setup() {
    Serial.begin(9600);
    /* Verifie si le oled se connecte sur le i2c */
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
       Serial.println(F("SSD1306 allocation failed"));
       for(;;);
    }
    delay(2000);
    /* Definie le texte et la couleur du texte sur le oled (couleur ne fonctionne pas) */
    display.setTextSize(1);
    display.setTextColor(WHITE);
    displayoled();
}
void loop() {
   delay(2000);
   /* Changer la variable refresholed a true pour rafraichir laffichage du oled */
   if(refresholed)
   {
        displayoled();
        refresholed = false;
   }
   
}