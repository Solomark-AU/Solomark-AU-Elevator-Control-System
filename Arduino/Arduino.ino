#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23XXX.h>
#include <Key.h>
#include <Keypad.h>

const int ELEVATORSPEED = 1;
const int HIGHOFFLOOR = 100;
const int MAXFLOORNUMBER = 4;
const int MAXSERVERNUMBER = 4;
const int MAXCONTAINERNUMBER = 4;
int ELEVATOR_NUMBER = 0;
Adafruit_MCP23X17 Core;

// status 电梯状态
// status::DOWNSIDE下行 STATIC不动 UPSIDE上行
enum status
{
    DOWNSIDE = -1,
    STATIC = 0,
    UPSIDE = 1
};

// engine 电机部分
// 包含电机ID及状态
struct engine
{
    int ID;
    status STATUS = STATIC;
};

void swap(int *src, int *val)
{
    int *temp;
    temp = src;
    val = src;
    src = temp;
}

template <typename T>
class Container
{
public:
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

    void clear() { this->length = 0; }

    void sort(int low, int high)
    {
        if (low < high)
        {
            int pivot = this->partition(low, high);
            this->sort(low, pivot - 1);
            this->sort(pivot + 1, high);
        }
    }

    T del(T value)
    {
        for (int i = 0; i < length; i++)
        {
            if (this->datas[i] == value)
            {
                return this->erase(i);
            }
        }
        return 0;
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
        for (int i = 0; i < this->length; i++)
            if (this->datas[i] == index)
                return true;
        return false;
    }

    T operator[](int index) { return this->datas[index]; }

private:
    int length = 0;
    T datas[MAXCONTAINERNUMBER];
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

Container<engine> engines;

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
        int index[2] = {0, 0};
        if (elevator1 == status::UPSIDE)
            index[0] = 13;
        else if (elevator1 == status::STATIC)
            index[0] = 11;
        else if (elevator1 == status::DOWNSIDE)
            index[0] = 0;
        if (elevator2 == status::UPSIDE)
            index[1] = 13;
        else if (elevator2 == status::STATIC)
            index[1] = 11;
        else if (elevator2 == status::DOWNSIDE)
            index[1] = 0;
        this->display_character(2, index[0], keepTime);
        this->display_character(4, index[1], keepTime);
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

    numberData datas[16] = {
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
        numberData(0, 0, 0, 0, 0, 0, 1), // character: - $index: 15 &using for debugging
    };
};

displayerElement Display(5, 6, 4, 3, 2, 13, 12, 11, 10, 9, 8, 7);

// 电梯类
class elevator
{
public:
    int ID;
    bool isAvailable = true, isOpen = false;

    elevator(int engine_H = 0, int engine_L = 0)
    {
        this->ID = ++ELEVATOR_NUMBER;
        this->engine[0] = engine_H;
        this->engine[1] = engine_L;
        for (int i = 0; i < MAXFLOORNUMBER; i++)
            this->count[i] = 0;
    }

    ~elevator() {}

    void add_target(int request, int target)
    {
        Serial.println("Add-target:(id, request, target, status)");
        Serial.println((int)&(this->ID));
        Serial.println(this->ID);
        Serial.println(request);
        Serial.println(target);

        if (this->floor < request)
            this->STATUS = status::UPSIDE;
        else
            this->STATUS = status::DOWNSIDE;
        if (this->isAvailable)
        {
            if (!this->target.empty())
                this->target.erase(0);
            Core.digitalWrite(this->engine[0], request < target ? HIGH : LOW);
            Core.digitalWrite(this->engine[1], request < target ? LOW : HIGH);
        }
        this->target.push_back(request);
        this->target.push_back(target);
        if (!this->isAvailable)
            this->target.sort(0, this->target.size());
        this->isAvailable = false;
        this->count[request - 1]++;

        Serial.println(this->STATUS);
        Serial.println("\n");
    }

    void move()
    {
        if ((this->high <= 0 && this->STATUS == status::DOWNSIDE) || (this->high >= HIGHOFFLOOR * MAXFLOORNUMBER && this->STATUS == status::UPSIDE))
        {
            Serial.println("Out of rangement:(ID)");
            Serial.println(this->ID);
            this->STATUS = status::STATIC;
            Core.digitalWrite(this->engine[0], LOW);
            Core.digitalWrite(this->engine[1], LOW);
        }
        else
        {
            this->high += this->STATUS * ELEVATORSPEED;
            this->floor = this->high / HIGHOFFLOOR + 1;
        }
        if (this->high % HIGHOFFLOOR == 0 && this->target == this->floor)
            open_door();
        for (int i = 0; i < MAXFLOORNUMBER; i++)
            this->station = this->count[this->station] <= this->count[i] ? this->station : i + 1;
        Serial.println("Info-Elevator:(*id, status, floor, high, targetNumbers)");
        Serial.println((int)&(this->ID));
        Serial.println(this->STATUS);
        Serial.println(this->floor);
        Serial.println(this->high);
        Serial.println(this->target.size());
        Serial.println("\n");
    }

    void open_door()
    {
        Serial.println("Open-door:");
        Serial.println(this->ID);
        Serial.println(this->floor);
        Serial.println(this->engine[0]);
        Serial.println("\n");

        Core.digitalWrite(this->engine[0], LOW);
        Core.digitalWrite(this->engine[1], LOW);
        this->target.del(this->floor);

        Serial.println(this->target.size());
        Serial.println(this->target[0]);
        delay(2000);

        if (this->target.empty())
        {
            this->isAvailable = true;
            if (this->floor != this->station)
            {
                this->target.push_back(this->station);
                this->STATUS = (this->station > this->floor ? status::UPSIDE : status::DOWNSIDE);
            }
            else
            {
                this->STATUS = status::STATIC;
                return;
            }
        }
        if (this->STATUS == status::UPSIDE)
        {
            if (this->target.back() == this->floor)
            {
                Core.digitalWrite(this->engine[0], LOW);
                Core.digitalWrite(this->engine[1], HIGH);
                this->STATUS = status::DOWNSIDE;
            }
            else
            {
                Core.digitalWrite(this->engine[0], HIGH);
                Core.digitalWrite(this->engine[1], LOW);
                this->STATUS = status::UPSIDE;
            }
        }
        else if (this->STATUS == status::DOWNSIDE)
        {
            if (this->target.front() == this->floor)
            {
                Core.digitalWrite(this->engine[0], HIGH);
                Core.digitalWrite(this->engine[1], LOW);
                this->STATUS = status::UPSIDE;
            }
            else
            {
                Core.digitalWrite(this->engine[0], LOW);
                Core.digitalWrite(this->engine[1], HIGH);
                this->STATUS = status::DOWNSIDE;
            }
        }
    }

    int get_floor() { return this->floor; }

    status get_status() { return this->STATUS; }

private:
    int floor = 1, station = 1, high = 0; // station 常驻楼层
    int engine[2], count[MAXFLOORNUMBER];
    status STATUS = status::STATIC;
    Container<int> target;
};

// elevatorWell 电梯井部分
class elevatorWell
{
public:
    elevatorWell() {}

    ~elevatorWell() {}

    int get_elevatorNumbers() { return WellNumber; }

    void register_elevator(int engine1, int engine2)
    {
        WellNumber++;
        this->group.push_back(elevator(engine1, engine2));
    }

private:
    int WellNumber = 1;
    Container<elevator> group;
};

Container<elevator *> elevators;
int inputNum[2] = {0, 0}; // RequestFloor,TargetFloor
bool mode = false;        // True:TargetFloor False:RequestFloor

byte colPins[4] = {A3, A2, A1, A0};
byte rowPins[4] = {6, 8, A5, A4};
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};
Keypad Key(makeKeymap(keymap), rowPins, colPins, 4, 4);

void setup()
{
    Serial.begin(115200);
    Core.begin_I2C();
    for (int i = 2; i < 14; i++)
        pinMode(i, OUTPUT);
    elevator temp_obj1(1, 2), temp_obj2(3, 4);
    elevator *ptr1 = &temp_obj1, *ptr2 = &temp_obj2;
    elevators.push_back(ptr1);
    elevators.push_back(ptr2);

    for (int i = 1; i <= 4; i++)
        Core.pinMode(i, OUTPUT);
}

void loop()
{
    Display.display_number((*elevators[0]).get_floor(), (*elevators[1]).get_floor());
    char key = Key.getKey();
    if (key == '*')
    {
        for (int i = 1; i <= 500; i++)
            Display.display_status((*elevators[0]).get_status(), (*elevators[1]).get_status());
        Serial.println("Info-Elevator:(*id, id, status, floor, high, targetNumbers)");
        Serial.println((int)&(elevators[0]->ID));
        Serial.println(elevators[0]->ID);
        Serial.println(elevators[0]->get_status());
        Serial.println(elevators[0]->get_floor());
        Serial.println("\n");
    }
    else if (key <= '9' && key >= '0')
    {
        inputNum[mode] = key - '0';
        if (mode)
        {
            for (int i = 1; i <= 300; i++)
                Display.display_number(inputNum[0], inputNum[1]);
            int minDistance = 1024, minDistanceNum = 0;
            for (int i = 0; i < elevators.size(); i++)
            {
                if ((*elevators[i]).isAvailable == 1 ||
                    ((*elevators[i]).get_status() * (inputNum[0] - (*elevators[i]).get_floor()) > 0 &&
                     (*elevators[i]).get_status() * (inputNum[1] - inputNum[0]) > 0))
                {
                    if (abs(inputNum[0] - (*elevators[i]).get_floor()) < minDistance)
                    {
                        minDistanceNum = i;
                        minDistance = abs(inputNum[0] - (*elevators[i]).get_floor());
                    }
                }
            }
            Serial.println("op-elevator:");
            Serial.println((int)((*elevators[minDistanceNum]).ID));
            (*elevators[minDistanceNum]).add_target(inputNum[0], inputNum[1]);
            mode = false;
        }
        else
        {
            mode = true;
            for (int i = 0; i < 300; i++)
                Display.display_character(4, inputNum[0], 1);
        }
    }
    for (int i = 0; i < elevators.size(); i++)
        (*elevators[i]).move();
}
