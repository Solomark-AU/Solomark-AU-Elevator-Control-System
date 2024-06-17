#include <Key.h>
#include <Keypad.h>

#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>

Adafruit_MCP23X17 Core;

enum status
{
    DOWNSIDE = -1,
    STATIC = 0,
    UPSIDE = 1
};

byte colPins[4] = {A3, A2, A1, A0};
byte rowPins[4] = {6, 8, A5, A4};
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
Keypad Key(makeKeymap(keymap), rowPins, colPins, 4, 4);

class displayerElement
{
public:
    displayerElement(
        int pin1, int pin2, int pin3, int pin4, int pin5, int pin6,
        int pin7, int pin8, int pin9, int pin10, int pin11, int pin12)
    {
        this->pinLight[0] = 0;
        this->pinLight[1] = pin1;
        this->pinLight[2] = pin2;
        this->pinLight[3] = pin3;
        this->pinLight[4] = pin4;
        this->pinLight[5] = pin5;
        this->pinLight[6] = pin6;
        this->pinLight[7] = pin7;
        this->pinLight[8] = pin8;
        this->pinLight[9] = pin9;
        this->pinLight[10] = pin10;
        this->pinLight[11] = pin11;
        this->pinLight[12] = pin12;
    }

    void set_numberControlPins(int number1, int number2, int number3, int number4)
    {
        this->controlNumber[1] = number1;
        this->controlNumber[2] = number2;
        this->controlNumber[3] = number3;
        this->controlNumber[4] = number4;
    }

    void set_lightControl(
        int light1, int light2, int light3, int light4,
        int light5, int light6, int light7, int light8)
    {
        this->numberLights[1] = light1;
        this->numberLights[2] = light2;
        this->numberLights[3] = light3;
        this->numberLights[4] = light4;
        this->numberLights[5] = light5;
        this->numberLights[6] = light6;
        this->numberLights[7] = light7;
        this->numberLights[8] = light8;
    }

    void display_character(int position, int number, int keepTime)
    {
        for (int i = 1; i <= 8; i++)
            digitalWrite(this->pinLight[this->numberLights[i]], this->datas[number].lights[i] ? LOW : HIGH);
        digitalWrite(this->pinLight[this->controlNumber[position]], HIGH);
        delay(keepTime);
        this->reset_pin();
    }

    void display_point(int position, int keepTime)
    {
        for (int i = 1; i < 8; i++)
            (this->pinLight[this->numberLights[i]], HIGH);
        digitalWrite(this->pinLight[this->numberLights[8]], LOW);
        digitalWrite(this->pinLight[this->controlNumber[position]], HIGH);
        delay(keepTime);
        this->reset_pin();
    }

    void display_status(status elevator1, status elevator2, int keepTime = 1)
    {
        int index[4] = {0, 0, 0, 0};
        if (elevator1 == UPSIDE)
        {
            index[0] = 13;
            index[1] = 14;
        }
        else if (elevator1 == STATIC)
        {
            index[0] = 11;
            index[1] = 12;
        }
        else if (elevator1 == DOWNSIDE)
        {
            index[0] = 10;
            index[1] = 0;
        }
        if (elevator2 == UPSIDE)
        {
            index[2] = 13;
            index[3] = 14;
        }
        else if (elevator2 == STATIC)
        {
            index[2] = 11;
            index[3] = 12;
        }
        else if (elevator2 == DOWNSIDE)
        {
            index[2] = 10;
            index[3] = 0;
        }
        for (int i = 0; i < 4; i++)
            this->display_character(i + 1, index[i], keepTime);
        this->display_point(2, keepTime);
        this->display_point(3, keepTime);
    }

    void reset_pin()
    {
        for (int i = 1; i <= 4; i++)
            digitalWrite(this->pinLight[this->controlNumber[i]], LOW);
        for (int i = 1; i <= 8; i++)
            digitalWrite(this->pinLight[this->numberLights[i]], HIGH);
    }

    void display_number(int elevator1, int elevator2, int keepTime = 1)
    {
        this->display_point(2, keepTime);
        this->display_point(3, keepTime);
        this->display_character(1, elevator1 / 10, keepTime);
        this->display_character(2, elevator1 % 10, keepTime);
        this->display_character(3, elevator2 / 10, keepTime);
        this->display_character(4, elevator2 % 10, keepTime);
    };

private:
    struct numberData
    {
        int numberID;
        bool lights[8];

        numberData(
            bool light1, bool light2, bool light3, bool light4,
            bool light5, bool light6, bool light7)
        {
            this->lights[0] = false;
            this->lights[1] = light1;
            this->lights[2] = light2;
            this->lights[3] = light3;
            this->lights[4] = light4;
            this->lights[5] = light5;
            this->lights[6] = light6;
            this->lights[7] = light7;
            this->lights[8] = false;
        }
    };

    int pinLight[13];
    int controlNumber[5] = {0, 11, 1, 2, 5};
    int numberLights[9] = {0, 9, 7, 6, 3, 4, 12, 10, 8};

    numberData datas[15] = {
        numberData(1, 1, 1, 1, 1, 1, 0), // number & character: 0 || O
        numberData(0, 0, 1, 1, 0, 0, 0), // number: 1
        numberData(0, 1, 1, 0, 1, 1, 1), // number: 2
        numberData(0, 1, 1, 1, 1, 0, 1), // number: 3
        numberData(1, 0, 1, 1, 0, 0, 1), // number: 4
        numberData(1, 1, 0, 1, 1, 0, 1), // number: 5
        numberData(1, 1, 0, 1, 1, 1, 1), // number: 6
        numberData(0, 1, 1, 1, 0, 0, 0), // number: 7
        numberData(1, 1, 1, 1, 1, 1, 1), // number: 8
        numberData(1, 1, 1, 1, 1, 0, 1), // number: 9
        numberData(1, 0, 0, 0, 1, 1, 0), // character: L $index: 10
        numberData(1, 1, 0, 1, 1, 0, 1), // character: S $index: 11
        numberData(1, 1, 1, 1, 0, 1, 1), // character: A $index: 12
        numberData(1, 0, 1, 1, 1, 1, 0), // character: U $index: 13
        numberData(1, 1, 1, 0, 0, 1, 1), // character: P $index: 14
    };
};

displayerElement Display(5, 6, 4, 3, 2, 13, 12, 11, 10, 9, 8, 7);

void setup()
{
    Serial.begin(9600);
    Core.begin_I2C();
    for (int i = 2; i < 14; i++)
        pinMode(i, OUTPUT);
}
void loop()
{
    char key = Key.getKey();
}
