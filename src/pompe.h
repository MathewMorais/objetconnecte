class Pompe{
    public:
        int pause; //EEPROM 2
        int duree; //EEPROM 3
        int state;
        void set_pause(int new_pause);
        void set_duree(int new_duree);
        void begin();
        int pin;
};