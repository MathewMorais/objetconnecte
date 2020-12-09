class Senseur
{
    public :
        float temperature_actuelle;
        int temperature_max; //EEPROM 0
        int temperature_min; //EEPROM 1
        void begin();
        void set_temperature_actuelle();
        void set_temperature_max(int temperature);
        void set_temperature_min(int temperature);
};