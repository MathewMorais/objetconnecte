/* Liste des includes pour l'oled */
#include <Arduino.h>
#include <WiFiManager.h>
// #include <string>
// #include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Adafruit_I2CDevice.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <time.h>
#include <mbedtls/md.h>
#include "sensor.h"
#include "oled.h"
#include "pompe.h"

using namespace std;

AsyncWebServer server(80);
WiFiManager wifiManager;
StaticJsonDocument<200> doc;
// OneWire oneWire(4);
// DallasTemperature sensors(&oneWire);

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_RESET -1

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";

char buffer [33];

time_t now;

Oled oled;
Senseur senseur;
Pompe pompe;

String tokenList[10];
String token;

int heatpadState = LOW;
int pinHeatpad = 25;
// int senseur.temperature_max; //EEPROM 0
// int senseur.temperature_min; //EEPROM 1
String aquariumNom;// EEPROM 4
String jsonresponse;

void display_oled()
{
    String pompe_string;
    if(digitalRead(pompe.pin) == HIGH)
    {
        pompe_string = "Ouvert : "+String(itoa(pompe.pause,buffer,10))+" minute(s)";
    }
    else
    {
        pompe_string = "Fermer : "+String(itoa(pompe.pause,buffer,10))+" minute(s)";
    }
    String string_array[] = 
    {
        "Nom : "+aquariumNom,
        "Temp. eau : "+String(senseur.temperature_actuelle),
        "Temp. cible : "+String(itoa(senseur.temperature_min,buffer,10))+" - "+String(itoa(senseur.temperature_max,buffer,10)),
        pompe_string,
        "IP : "+WiFi.localIP().toString()
    };
    oled.set_display(string_array,5);
}
void handle_root(AsyncWebServerRequest *request) {
    aquariumNom = EEPROM.readString(4);

    if(request->hasParam("aquariumNom"))
    {
        senseur.set_temperature_max(atoi(request->getParam("senseur.temperature_max")->value().c_str()));
        senseur.set_temperature_min(atoi(request->getParam("senseur.temperature_min")->value().c_str()));
        pompe.set_pause(atoi(request->getParam("pompePause")->value().c_str()));
        pompe.set_duree(atoi(request->getParam("pompeDuree")->value().c_str()));
        aquariumNom = request->getParam("aquariumNom")->value();
        EEPROM.writeString(4, aquariumNom);
        EEPROM.commit();
    }

    String temperature_actuelle_string = String(senseur.temperature_actuelle);
    String temperature_min_string = String(itoa(senseur.temperature_min,buffer,10));
    String temperature_max_string = String(itoa(senseur.temperature_max,buffer,10));
    String pompe_pause_string = String(itoa(pompe.pause,buffer,10));
    String pompe_duree_string = String(itoa(pompe.duree,buffer,10));

    String HTML = "<!doctype html>";
    HTML += "<html>";
    HTML += "<meta charset=\"utf-8\"/>";
    HTML += "<header><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\" integrity=\"sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z\" crossorigin=\"anonymous\"></header>";
    HTML += "<head>";
    HTML += "<h2 class=\"text-center p-5\"><u>Options aquarium</u></h2>";
    HTML += "</head>";
    HTML += "<body>";
    HTML += "<form action=\"/\" class=\"container\">";
    HTML += "<div class=\"form-group\"><label>Nom de l'aquarium</label><input class=\"form-control\" type=\"text\" name='aquariumNom' value=\""+aquariumNom+"\" /></div>";
    HTML += "<div class=\"form-group\"><label>Température actuelle</label><input class=\"form-control\" type=\"number\" value=\""+temperature_actuelle_string+"\" readonly/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible minimum</label><input class=\"form-control\" type=\"number\" name=\"senseur.temperature_min\" value=\""+temperature_min_string+"\"/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible maximum</label><input class=\"form-control\" type=\"number\" name=\"senseur.temperature_max\" value=\""+temperature_max_string+"\"/></div>";
    HTML += "<div class=\"form-group\"><label>Pause de la pompe (en minutes)</label><input class=\"form-control\" type='number' name='pompePause' value=\""+pompe_pause_string+"\"></div>";
    HTML += "<div class=\"form-group\"><label>Durée de la pompe (en minutes)</label><input class=\"form-control\" type='number' name='pompeDuree' value=\""+pompe_duree_string+"\"></div>";
    HTML += "<div class=\"form-group\"><button class=\"btn btn-primary\">Confirmer</button></div>";
    HTML += "</form>";
    HTML += "</body>";
    HTML += "</html>";
    request->send(200, "text/html", HTML);
} 
String random_string(std::size_t length)
{
    const string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    random_device random_device;
    mt19937 generator(random_device());
    uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    String random_string;

    for (size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}
bool checkToken(String tokenheader)
{
    if(tokenheader == token)
    {
        return true;
    }
    return false;
}
void flushTokens()
{
    for(int i = 0; i < sizeof(tokenList); i++)
    {
        tokenList[i] = "";
    }
}
void setup() {
    Serial.begin(9600);
    EEPROM.begin(512);
    senseur.begin();
    oled.begin();
    pompe.begin();

    pinMode(pinHeatpad, OUTPUT);// pin pompe
    digitalWrite(pinHeatpad, LOW);

    // senseur.temperature_max = EEPROM.read(0);
    // senseur.temperature_min = EEPROM.read(1);
    aquariumNom = EEPROM.readString(4);
    WiFi.mode(WIFI_STA);
    wifiManager.autoConnect(ssid,password);
    delay(2000);

    server.begin();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      handle_root(request);
    });
    server.on("/obtenirToken", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        if(request->getParam("username")->value() == "admin" && request->getParam("password")->value() == "admin")
        {
            doc["_token"] = token = String(time(&now)) + String("_de") + String(random_string(15));
        }
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCelsius", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["temp"] = String(senseur.temperature_actuelle);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCibleMin", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["tempMin"] = String(senseur.temperature_min);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCibleMax", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["tempMax"] = String(senseur.temperature_max);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirPausePompe", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["pause"] = String(pompe.pause);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirDureePompe", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["duree"] = String(pompe.duree);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirNomAquarium", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["nom"] = String(aquariumNom);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });

    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        bool tokenok = checkToken(String(request->getHeader("_token")->value()));
        deserializeJson(doc, (const char*)data);
        if(request->url() == "/choisirTemperatureCibleMax" && tokenok){
            if(request->method() == HTTP_POST)
            {
                senseur.set_temperature_max(atoi(doc["tempMax"]));
                doc.clear();
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de la température maximum réussi\"}");
            }
        }
        else if(request->url() == "/choisirTemperatureCibleMin" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                senseur.set_temperature_min(atoi(doc["tempMin"]));
                doc.clear();
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de la température minimum réussi\"}");
            }
        }
        else if(request->url() == "/choisirPausePompe" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                pompe.set_pause(atoi(doc["pause"]));
                doc.clear();
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de pause réussi\"}");
            }
        }
        else if(request->url() == "/choisirDureePompe" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                pompe.set_duree(atoi(doc["duree"]));
                doc.clear();
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de durée réussi\"}");
            }
        }
        else if(request->url() == "/choisirNomAquarium" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                aquariumNom = String(atoi(doc["nom"]));
                doc.clear();
                EEPROM.writeString(4, aquariumNom);
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de nom réussi\"}");
            }
        }
        else
        {
            jsonresponse = "";
            doc.clear();
            Serial.printf("NOT_FOUND: ");
            if(request->method() == HTTP_GET)
            doc["GET"] = "Not Found";
            else if(request->method() == HTTP_POST)
            doc["POST"] = "Not Found";
            else if(request->method() == HTTP_DELETE)
            doc["DELETE"] = "Not Found";
            else if(request->method() == HTTP_PUT)
            doc["PUT"] = "Not Found";
            else if(request->method() == HTTP_PATCH)
            doc["PATCH"] = "Not Found";
            else if(request->method() == HTTP_HEAD)
            doc["HEAD"] = "Not Found";
            else if(request->method() == HTTP_OPTIONS)
            doc["OPTIONS"] = "Not Found";
            else
            doc["UNKNOWN"] = "Not Found";
            
            serializeJsonPretty(doc,jsonresponse);
            doc.clear();
            request->send(404, "application/json", jsonresponse);
        }
    });
    delay(100);
}
void loop() {
    senseur.set_temperature_actuelle();
    //Serial.println(String(senseur.temperature_actuelle));
    
    if(senseur.temperature_actuelle > float(senseur.temperature_max)+1)
    {
        if(digitalRead(pinHeatpad) == HIGH)
        {
            digitalWrite(pinHeatpad, LOW);
        }
    }
    else if(senseur.temperature_actuelle < float(senseur.temperature_min))
    {
        if(digitalRead(pinHeatpad) == LOW)
        {
            digitalWrite(pinHeatpad, HIGH);
        }
    }
    if(digitalRead(pompe.pin) == HIGH)
    {
        Serial.println("Ouvert : "+String((time(&now) % (pompe.pause * 60))));
        //if(dureePompeTimer >= (pompeDuree*60000))
        if( (time(&now) % (pompe.duree * 60)) == 0)
        {
            digitalWrite(pompe.pin, LOW);
            delay(3000);
        }
    }
    else if(digitalRead(pompe.pin) == LOW)
    {
        Serial.println("Ferme : "+String((time(&now) % (pompe.pause * 60))));
        //if(PausePompeTimer  >= (pompePause*60000)) 
        if( (time(&now) % (pompe.pause * 60)) == 0)
        {
            digitalWrite(pompe.pin, HIGH);
            delay(3000);
        }
    }
    display_oled();
    delay(250);
}