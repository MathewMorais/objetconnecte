#include <WebServer.h>
#include <WiFiManager.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
using namespace std;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";
int tempcible = 25;
String filtrecadence = "30 minutes";
WebServer server(80);
WiFiManager wifiManager;
void displayoled()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Temp. actue : 12");
    display.setCursor(0, 20);
    char buffer [33];
    char *intStr = itoa(tempcible,buffer,10);
    String str = String(intStr);
    display.println("Temp. cible : "+str);
    display.setCursor(0, 35);
    display.print("Filtre : "+filtrecadence);
    display.setCursor(0, 50);
    IPAddress ip = WiFi.localIP();
    display.print("IP : ");
    display.print(ip);
    display.display();
}
void handle_root() {
    String filtreselected = "<option>15 minutes</option><option>30 minutes</option><option>1 heure</option><option>2 heures</option>";
    if(server.hasArg("tempcible"))
    {
        int temptocheck = atoi(server.arg("tempcible").c_str());
        filtrecadence = server.arg("filtrecadence");
        if(temptocheck > 10 && temptocheck < 40)
        {
            tempcible = temptocheck;
        }
        if(filtrecadence == "15 minutes")
        {
            filtreselected = "<option selected=''>15 minutes</option><option>30 minutes</option><option>1 heure</option><option>2 heures</option>";
        }
        else if(filtrecadence == "30 minutes")
        {
            filtreselected = "<option>15 minutes</option><option selected=''>30 minutes</option><option>1 heure</option><option>2 heures</option>";
        }
        else if(filtrecadence == "1 heure")
        {
            filtreselected = "<option>15 minutes</option><option>30 minutes</option><option selected=''>1 heure</option><option>2 heures</option>";
        }
        else if(filtrecadence == "2 heures")
        {
            filtreselected = "<option>15 minutes</option><option>30 minutes</option><option>1 heure</option><option selected=''>2 heures</option>";
        }
    }
    char buffer [33];
    //char *intStr = itoa(tempcible,buffer,10);
    String strtmp = String(itoa(tempcible,buffer,10));
    String HTML = "<!doctype html>";
    HTML += "<html>";
    HTML += "<meta charset='utf-8'/>";
    HTML += "<header><link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css' integrity='sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z' crossorigin='anonymous'></header>";
    HTML += "<head>";
    HTML += "<h2 class=\"text-center p-5\"><u>Options aquarium</u></h2>";
    HTML += "</head>";
    HTML += "<body>";
    HTML += "<form action='/' class=\"container\">";
    HTML += "<div class=\"form-group\"><label>Température actuelle</label><input class=\"form-control\" type=\"number\" value=\"12\" readonly/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible</label><input class=\"form-control\" type=\"number\" name='tempcible' value='"+strtmp+"'/></div>";
    HTML += "<div class=\"form-group\"><label>Cadence de filtre</label><select name='filtrecadence' class=\"form-control\">"+filtreselected+"</select></div>";
    HTML += "<div class=\"form-group\"><button class=\"btn btn-primary\">Confirmer</button></div>";
    HTML += "</form>";
    HTML += "</body>";
    HTML += "</html>";
    displayoled();
    server.send(200, "text/html", HTML);
} 
void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    wifiManager.autoConnect(ssid,password);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    delay(2000);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    displayoled();
    server.begin();
    server.on("/", handle_root);
    delay(100);
}
void loop() {
    server.handleClient();
}