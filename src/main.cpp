/* Liste des includes pour l'oled */
#include <Arduino.h>
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
#include <ESPAsyncWebServer.h>
using namespace std;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";
int tempcible = 25;
float tempC;
int heatpadState = LOW;
bool refresholed = false;
String optionsFiltre [4] = {"15 minutes", "30 minutes", "1 heure", "2 heure"}; 
char buffer [33];
String filtrecadence = "30 minutes";
AsyncWebServer server(80);
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
    String tempCstr = String(tempC);
    display.println("Temp. actue : "+tempCstr);
    display.setCursor(0, 20);
    String tempciblestr = String(itoa(tempcible,buffer,10));
    display.println("Temp. cible : "+tempciblestr);
    display.setCursor(0, 35);
    display.print("Filtre : "+filtrecadence);
    display.setCursor(0, 50);
    IPAddress ip = WiFi.localIP();
    display.print("IP : ");
    display.print(ip);
    display.display();
}
void handle_root(AsyncWebServerRequest *request) {
    String filtreselected = "";
    String tempCstr = String(tempC);
    if(request->hasParam("tempcible"))
    {
        tempcible = atoi(request->getParam("tempcible")->value().c_str());
        filtrecadence = request->getParam("filtrecadence")->value();
    }
    for(String ele : optionsFiltre)
    {
        if(ele == filtrecadence)
        {
            filtreselected = filtreselected+"<option selected>"+ele+"</option>";
        }
        else
        {
            filtreselected = filtreselected+"<option>"+ele+"</option>";
        }
    }
    String tempciblestr = String(itoa(tempcible,buffer,10));
    String HTML = "<!doctype html>";
    HTML += "<html>";
    HTML += "<meta charset=\"utf-8\"/>";
    HTML += "<header><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" integrity=\"sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z\" crossorigin=\"anonymous\"></header>";
    HTML += "<head>";
    HTML += "<h2 class=\"text-center p-5\"><u>Options aquarium</u></h2>";
    HTML += "</head>";
    HTML += "<body>";
    HTML += "<form action=\"/\" class=\"container\">";
    HTML += "<div class=\"form-group\"><label>Nom de l'aquarium</label><input class=\"form-control\" type=\"text\" value=\"\" /></div>";
    HTML += "<div class=\"form-group\"><label>Température actuelle</label><input class=\"form-control\" type=\"number\" value=\""+tempCstr+"\" readonly/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible</label><input class=\"form-control\" type=\"number\" name=\"tempcible\" value=\""+tempciblestr+"\"/></div>";
    HTML += "<div class=\"form-group\"><label>Cadence de filtre</label><select name=\"filtrecadence\" class=\"form-control\">"+filtreselected+"</select></div>";
    HTML += "<div class=\"form-group\"><label>Durée de la filtration (minutes)</label><input class=\"form-control\" type=\"number\" value=\"\" /></div>";
    HTML += "<div class=\"form-group\"><button class=\"btn btn-primary\">Confirmer</button></div>";
    HTML += "</form>";
    HTML += "</body>";
    HTML += "</html>";
    refresholed = true;
    request->send(200, "text/html", HTML);
} 
void setup() {
    Serial.begin(9600);
    sensors.begin(); 
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
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      handle_root(request);
    });
    pinMode(25, OUTPUT);
    delay(100);
}
void loop() {
    //tempC = sensors.getTempC(insideThermometer);
    // if(tempC > tempcible)
    // {
    //     if(heatpadState == HIGH)
    //     {
    //         heatpadState = LOW;
    //         digitalWrite(25, heatpadState);
    //     }
    // }
    // if(tempC < tempcible)
    // {
    //     if(heatpadState == LOW)
    //     {
    //         heatpadState = HIGH;
    //         digitalWrite(25, heatpadState);
    //     }
    // }
    if(refresholed)
    {
        displayoled();
        Serial.print("ok");
    }
    delay(2000);
}