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
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <time.h>
#include <mbedtls/md.h>

using namespace std;

AsyncWebServer server(80);
WiFiManager wifiManager;
StaticJsonDocument<200> doc;
OneWire oneWire(4);
DallasTemperature sensors(&oneWire);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Esp32Aquarium";
const char* password = "Patate123";

char buffer [33];
float tempCelsius;

time_t now;

String tokenList[10];
String token;

int heatpadState = LOW;
int pompeState = LOW;
int pinHeatpad = 25;
int pinPompe = 26;
int tempCibleMax; //EEPROM 0
int tempCibleMin; //EEPROM 1
int pompePause;// EEPROM 2
int pompeDuree;// EEPROM 3
String aquariumNom;// EEPROM 4
String jsonresponse;

void displayoled()
{
    String tempCelsiusStr = String(tempCelsius);
    String tempCibleStr = String(itoa(tempCibleMin,buffer,10))+" - "+String(itoa(tempCibleMax,buffer,10));
    String pompePauseStr = String(itoa(pompePause,buffer,10));
    IPAddress ip = WiFi.localIP();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Nom : "+aquariumNom);
    display.setCursor(0, 20);
    display.print("Temp. eau : "+tempCelsiusStr);
    display.setCursor(0, 30);
    display.print("Temp. cible : "+tempCibleStr);
    display.setCursor(0, 40);
    if(digitalRead(pinPompe) == HIGH)
    {
        display.print("Ouvert : "+pompePauseStr+" minute(s)");
    }
    else
    {
        display.print("Fermer : "+pompePauseStr+" minute(s)");
    }
    display.setCursor(0, 50);
    display.print("IP : ");
    display.print(ip);
    display.display();
}
void handle_root(AsyncWebServerRequest *request) {
    tempCibleMax = EEPROM.read(0);
    tempCibleMin = EEPROM.read(1);
    pompePause = EEPROM.read(2);
    pompeDuree = EEPROM.read(3);
    aquariumNom = EEPROM.readString(4);

    if(request->hasParam("aquariumNom"))
    {
        tempCibleMax = atoi(request->getParam("tempCibleMax")->value().c_str());
        EEPROM.write(0, tempCibleMax);
        tempCibleMin = atoi(request->getParam("tempCibleMin")->value().c_str());
        EEPROM.write(1, tempCibleMin);
        pompePause = atoi(request->getParam("pompePause")->value().c_str());
        EEPROM.write(2, pompePause);
        pompeDuree = atoi(request->getParam("pompeDuree")->value().c_str());
        EEPROM.write(3, pompeDuree);
        aquariumNom = request->getParam("aquariumNom")->value();
        EEPROM.writeString(4, aquariumNom);
        EEPROM.commit();
    }

    String tempCelsiusStr = String(tempCelsius);
    String tempCibleMinStr = String(itoa(tempCibleMin,buffer,10));
    String tempCibleMaxStr = String(itoa(tempCibleMax,buffer,10));
    String pompePauseStr = String(itoa(pompePause,buffer,10));
    String pompeDureeStr = String(itoa(pompeDuree,buffer,10));

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
    HTML += "<div class=\"form-group\"><label>Température actuelle</label><input class=\"form-control\" type=\"number\" value=\""+tempCelsiusStr+"\" readonly/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible minimum</label><input class=\"form-control\" type=\"number\" name=\"tempCibleMin\" value=\""+tempCibleMinStr+"\"/></div>";
    HTML += "<div class=\"form-group\"><label>Température cible maximum</label><input class=\"form-control\" type=\"number\" name=\"tempCibleMax\" value=\""+tempCibleMaxStr+"\"/></div>";
    HTML += "<div class=\"form-group\"><label>Pause de la pompe (en minutes)</label><input class=\"form-control\" type='number' name='pompePause' value=\""+pompePauseStr+"\"></div>";
    HTML += "<div class=\"form-group\"><label>Durée de la pompe (en minutes)</label><input class=\"form-control\" type='number' name='pompeDuree' value=\""+pompeDureeStr+"\"></div>";
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
// void addToken(String token)
// {
//     for(int i = 0; i < sizeof(tokenList); i++)
//     {
//         if(tokenList[i].length() < 4)
//         {
//             tokenList[i] = token;
//         }
//     }
// }
bool checkToken(String tokenheader)
{
    // for(int i = 0; i < sizeof(tokenList); i++)
    // {
    //     if(tokenList[i] == token)
    //     {
    //         return true;
    //     }
    // }
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
    sensors.begin();
    EEPROM.begin(512);

    pinMode(pinHeatpad, OUTPUT);// pin pompe
    pinMode(pinPompe, OUTPUT);// pin heatpad
    digitalWrite(pinHeatpad, LOW);
    digitalWrite(pinPompe, LOW);

    tempCibleMax = EEPROM.read(0);
    tempCibleMin = EEPROM.read(1);
    pompePause = EEPROM.read(2);
    pompeDuree = EEPROM.read(3);
    aquariumNom = EEPROM.readString(4);

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
    server.on("/obtenirToken", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        if(request->getParam("username")->value() == "admin" && request->getParam("password")->value() == "admin")
        {
            token = String(time(&now)) + String("_de") + String(random_string(15));
            doc["_token"] = token;
        }
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCelsius", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["tempCelsius"] = String(tempCelsius);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCibleMin", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["tempCibleMin"] = String(tempCibleMin);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirTemperatureCibleMax", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["tempCibleMax"] = String(tempCibleMax);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirPausePompe", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["pause"] = String(pompePause);
        serializeJsonPretty(doc,jsonresponse);
        doc.clear();
        request->send(200, "application/json", jsonresponse);
    });
    server.on("/obtenirDureePompe", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonresponse = "";
        doc["duree"] = String(pompeDuree);
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
                tempCibleMax = atoi(doc["tempCelsius"]);
                doc.clear();
                EEPROM.write(0, tempCibleMax);
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de la température maximum réussi\"}");
            }
        }
        else if(request->url() == "/choisirTemperatureCibleMin" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                tempCibleMin = atoi(doc["tempCelsius"]);
                doc.clear();
                EEPROM.write(1, tempCibleMin);
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de la température minimum réussi\"}");
            }
        }
        else if(request->url() == "/choisirPausePompe" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                pompePause = atoi(doc["pause"]);
                doc.clear();
                EEPROM.write(2, pompePause);
                EEPROM.commit();
                request->send(200, "application/json", "{\"message\":\"Changement de pause réussi\"}");
            }
        }
        else if(request->url() == "/choisirDureePompe" && tokenok)
        {
            if(request->method() == HTTP_POST)
            {
                pompeDuree = atoi(doc["duree"]);
                doc.clear();
                EEPROM.write(3, pompeDuree);
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
    sensors.requestTemperatures();
    tempCelsius = sensors.getTempCByIndex(0);
    //Serial.println(String(tempCelsius));
    
    if(tempCelsius > float(tempCibleMax)+1)
    {
        if(digitalRead(pinHeatpad) == HIGH)
        {
            digitalWrite(pinHeatpad, LOW);
        }
    }
    else if(tempCelsius < float(tempCibleMin))
    {
        if(digitalRead(pinHeatpad) == LOW)
        {
            digitalWrite(pinHeatpad, HIGH);
        }
    }
    if(digitalRead(pinPompe) == HIGH)
    {
        //Serial.println("Ouvert : "+String((time(&now) % (pompePause * 60))));
        //if(dureePompeTimer >= (pompeDuree*60000))
        if( (time(&now) % (pompeDuree * 60)) == 0)
        {
            digitalWrite(pinPompe, LOW);
            delay(1000);
        }
    }
    else if(digitalRead(pinPompe) == LOW)
    {
        //Serial.println("Ferme : "+String((time(&now) % (pompePause * 60))));
        //if(PausePompeTimer  >= (pompePause*60000)) 
        if( (time(&now) % (pompePause * 60)) == 0)
        {
            digitalWrite(pinPompe, HIGH);
            delay(1000);
        }
    }
    displayoled();
    delay(250);
}