#include <EEPROM.h>
class Pompe{
    public:
        int pause; //EEPROM 2
        int duree; //EEPROM 3
        int state;
        void set_pause(int new_pause);
        void set_duree(int new_duree);
        void begin();
        Pompe();
        int pin;
};

void Pompe::begin()
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    pause = EEPROM.read(2);
    duree = EEPROM.read(3);
}
void Pompe::set_pause(int new_pause){
    pause = new_pause;
    EEPROM.write(2, new_pause);
}
void Pompe::set_duree(int new_duree){
    duree = new_duree;
    EEPROM.write(3, new_duree);
}