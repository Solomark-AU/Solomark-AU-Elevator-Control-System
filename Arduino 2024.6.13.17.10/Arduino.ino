#include <MCP23017.h>

const int ELEVATORSPEED = 1;
const int HIGHOFBUILDING = 100;
const int MAXSERVERNUMBER = 4;
const int MAXCONTAINERNUMBER = 4;
int ELEVATOR_NUMBER = 0;
MCP23017 Core0(0x20);

// status 电梯状态
// DOWNSIDE下行 STATIC不动 UPSIDE上行
enum status
{
    DOWNSIDE = -1,
    STATIC = 0,
    UPSIDE = 1
};

template <typename T>
void swap(T *src, T *val)
{
    T *temp;
    temp = src;
    val = src;
    src = temp;
}

template <typename T>
class Container
{
public:
    int length = 0;
    T datas[MAXCONTAINERNUMBER];

    Container() {}

    ~Container() {}

    int size() { return this->length; }

    bool empty() { return this->length == 0; }

    void push_back(T value) { this->datas[this->length++] = value; }

    void insert(int index, T value)
    {
        for (int i = this->length++; i >= index; i--)
            this->datas[i + 1] = this->datas[i];
        this->datas[index] = value;
    }

    void sort(int low, int high)
    {
        if (low < high)
        {
            int pivot = this->partition(low, high);
            this->sort(low, pivot - 1);
            this->sort(pivot + 1, high);
        }
    }

    T erase(int index)
    {
        T result = datas[index];
        for (int i = index; i < length; i++)
            datas[i] = datas[i + 1];
        length--;
        return result;
    }

    T front() { return this->datas[0]; }

    T back() { return this->datas[this->length - 1]; }

    bool operator==(int index)
    {
        if (this->length == 0)
            return false;
        for (int i = 0; i < this->length - 1; i++)
            if (this->data[i] == index)
                return true;
        return false;
    }

    T operator[](int index) { return this->datas[index]; }

private:
    int partition(int low, int high)
    {
        T pivotKey = this->datas[low];
        while (low < high)
        {
            while ((low < high) && (pivotKey <= this->datas[high]))
                --high;
            swap(this->datas[low], this->datas[high]);
            while ((low < high) && (this->datas[low] <= pivotKey))
                ++low;
            swap(this->datas[low], this->datas[high]);
        }
        this->datas[low] = pivotKey;
        return low;
    }
};

// 电梯类
class elevator
{
public:
    bool isAvailable = true, isOpen = false;

    elevator() { this->ID = ++ELEVATOR_NUMBER; }

    ~elevator() {}

    void move()
    {
        this->high = this->STATUS * ELEVATORSPEED;
        this->floor = (this->high - this->high % HIGHOFBUILDING) % HIGHOFBUILDING + 1;
    }

    void open_door()
    {
    }

    int get_floor() { return this->floor; }

    status get_status() { return this->STATUS; }

private:
    int ID, floor = 1, station = 1, high; // station 常驻楼层
    status STATUS = STATIC;
    Container<int> target;
};

// elevatorWell 电梯井部分
class elevatorWell
{
public:
    elevatorWell()
    {
        register_elevator();
    }
    ~elevatorWell() {}

    int get_elevatorNumbers() { return WellNumber; };

    void register_elevator()
    {
        WellNumber++;
        this->group.push_back(elevator());
    };

private:
    int WellNumber = 1;
    Container<elevator> group;
};

// 楼层显示部分
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

class keypadElement
{
public:
    keypadElement() {}

    keypadElement(int col1, int col2, int col3, int col4,
                  int row1, int row2, int row3, int row4)
    {
        this->colPins[0] = col1;
        this->colPins[1] = col2;
        this->colPins[2] = col3;
        this->colPins[3] = col4;
        this->rowPins[0] = row1;
        this->rowPins[1] = row2;
        this->rowPins[2] = row3;
        this->rowPins[3] = row4;
    }

    ~keypadElement() {}

    void reset_pin()
    {
        for (int i = 0; i < 4; i++)
        {
            Core0.digitalWrite(this->colPins[i], HIGH);
            Core0.digitalWrite(this->rowPins[i], HIGH);
        }
    }

    void set_keypadReactionSeqTime(int ms) { this->seqTime = ms; }

    char get_character()
    {
        char press = '%';
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                Core0.digitalWrite(colPins[j], LOW);
                byte scan = Core0.digitalRead(rowPins[i]);
                if (scan == LOW)
                {
                    press = this->keymap[i][j];
                    Serial.println(this->keymap[i][j]);
                    delay(this->seqTime);
                    Core0.digitalWrite(colPins[j], HIGH);
                    break;
                }
                Core0.digitalWrite(colPins[j], HIGH);
            }
        }
        return press;
    }

private:
    int seqTime = 100;
    byte colPins[4] = {1, 2, 3, 4};
    byte rowPins[4] = {12, 11, 9, 8};
    const char keymap[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
};

displayerElement Displayer(9, 10, 11, 12, 13, 8, 7, 6, 5, 4, 3, 2);
keypadElement keypad;
Container<elevator> elevators;
int inputNum[2] = {0, 0}; // RequestFloor,TargetFloor
bool mode = 0;            // True:TargetFloor False:RequestFloor

void Update()
{
    for (int i = 1; i <= 100; i++)
        Displayer.display_number(elevators[0].get_floor(), elevators[1].get_floor());
    // 透過Keypad物件的getKey()方法讀取按鍵的字元
    char key = keypad.get_character();
    if (key)
    { // 若有按鍵被按下…
        if (key == '*')
            Displayer.display_status(elevators[0].get_status(), elevators[0].get_status());
        else if (key == '#')
        {
            if (mode = true)
            {
                // Algorithm...

                inputNum[0] = inputNum[1] = 0;
                mode = false;
            }
            else
                mode = true;
        }
        else if (key >= '0' && key <= '9')
        {
            inputNum[mode] *= 10;
            inputNum[mode] += key - '0';
        }
    }
}

void setup()
{
    Serial.begin(9600);
    /*
    for (int i = 2; i <= 13; i++)
        pinMode(i, OUTPUT);
    */
    Core0.init();
    Core0.portMode(MCP23017Port::A, OUTPUT);
    /*for (int i = 0; i < 8; i++)
        Core0.pinMode(i, OUTPUT);
    // Displayer.reset_pin();*/
    // keypad.reset_pin();
    for (int i = 0; i < 8; i++)
        Core0.digitalWrite(i, HIGH);
}

void loop()
{
    // Update();
    // Displayer.display_status(UPSIDE, DOWNSIDE, 1);
    // keypad.get_character();
}
