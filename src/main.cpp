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
#include <OneWire.h>
#include <DallasTemperature.h>
using namespace std;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";
int tempcible = 25;
String temp;
String filtrecadence = "30 minutes";
WebServer server(80);
WiFiManager wifiManager;
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 34

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;
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
    if(server.hasArg("relay"))
    {
        if(server.arg("relay") == "1")
        {
            digitalWrite(26, HIGH);
        }
        else
        {
            digitalWrite(26, LOW);
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
    HTML += "<div class=\"form-group\"><label>Nom de l'aquarium</label><input class=\"form-control\" type=\"text\" value=\"\" /></div>";
    HTML += "<div class=\"form-group\"><label>Température actuelle</label><input class=\"form-control\" type=\"number\" value=\"12\" readonly/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible</label><input class=\"form-control\" type=\"number\" name='tempcible' value='"+strtmp+"'/></div>";
    HTML += "<div class=\"form-group\"><label>Cadence de filtre</label><select name='filtrecadence' class=\"form-control\">"+filtreselected+"</select></div>";
    HTML += "<div class=\"form-group\"><label>Durée de la filtration (minutes)</label><input class=\"form-control\" type=\"number\" value=\"\" /></div>";
    HTML += "<div class=\"form-group\"><label>Relais on/off</label><input name='relay' class=\"form-control\" type=\"checkbox\" value=\"0\" /></div>";
    HTML += "<div class=\"form-group\"><button class=\"btn btn-primary\">Confirmer</button></div>";
    HTML += "</form>";
    HTML += "</body>";
    HTML += "</html>";
    displayoled();
    server.send(200, "text/html", HTML);
} 
void setup() {
    Serial.begin(9600);
    sensors.begin();
    // locate devices on the bus
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");
    // WiFi.mode(WIFI_STA);
    // wifiManager.autoConnect(ssid,password);
    // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    //     Serial.println(F("SSD1306 allocation failed"));
    //     for(;;);
    // }
    delay(2000);
    //pinMode(26, OUTPUT);
    //pinMode(34, INPUT);
    //digitalWrite(26, HIGH);
    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // displayoled();
    // server.begin();
    // server.on("/", handle_root);
    // delay(100);
}
void loop() {
    // server.handleClient();
    // temp = digitalRead(34);
    // Serial.print(temp);
    float tempC = sensors.getTempC(insideThermometer);
    Serial.print(tempC);
     delay(2000);
}