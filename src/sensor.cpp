#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);

class Senseur{
    public:
        float temperature_actuelle;
        int temperature_max; //EEPROM 0
        int temperature_min; //EEPROM 1
        void set_temperature_actuelle();
        Senseur(){};
        void begin();
        void set_temperature_max(int temperature);
        void set_temperature_min(int temperature);
};
void Senseur::begin()
{
    sensors.begin();
    temperature_max = EEPROM.read(0);
    temperature_min = EEPROM.read(1);
}
void Senseur::set_temperature_actuelle(){
    sensors.requestTemperatures();
    temperature_actuelle = sensors.getTempCByIndex(0);
    Serial.print(temperature_actuelle);
}
void Senseur::set_temperature_max(int temperature){
    temperature_max = temperature;
    EEPROM.write(0, temperature);
}
void Senseur::set_temperature_min(int temperature){
    temperature_min = temperature;
    EEPROM.write(1, temperature);
}
