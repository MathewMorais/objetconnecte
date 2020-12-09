#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

class Oled{
    public:
        void set_display(String string_array[], int size_of_array);
        Oled(){};
        void begin();
};

void Oled::begin(){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextSize(1);
    display.setTextColor(WHITE);
}
void Oled::set_display(String string_array[], int size_of_array){
    int cursor = 0;
    
    display.clearDisplay();
    //     Serial.println(string_array[1]);
    display.setCursor(0, 0);
    cursor += 20;
    for(int i = 0;i < size_of_array; i++)
    {
        // Serial.println(string_array[i]);
        display.print(string_array[i]);
        display.setCursor(0, cursor);
        cursor += 10;
    }
    display.display();
}
