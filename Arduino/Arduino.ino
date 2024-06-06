const int MAXSERVERNUMBER = 8;
const int MAXCONTAINERNUMBER = 16;
int ELEVATOR_NUMBER = 0;

// status 电梯状态
// DOWNSIDE下行 STATIC不动 UPSIDE上行
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
    // 为解决bug设置
};

template <typename T_>
void swap(T_ *src, T_ *val) {
    T_ *temp;
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

    Container() { }

    ~Container() { }

    int size() { return length; }

    void push_back(T value) { datas[length++] = value; }

    void insert(int index, T value)
    {
        length++;
        for (int i = index; i < length; i++)
            datas[i] = datas[i + 1];
        datas[index] = value;
    }

    int partition(int low, int high)
    {
        T pivotKey = datas[low];
        while (low < high)
        {
            while ((low < high) && (pivotKey <= datas[high]))
                --high;
            swap(datas[low], datas[high]);
            while ((low < high) && (datas[low] <= pivotKey))
                ++low;
            swap(datas[low], datas[high]);
        }
        datas[low] = pivotKey;
        return low;
    }

    void sort(int low, int high)
    {
        if (low < high)
        {
            int pivot = partition(datas, low, high);
            sort(low, pivot - 1);
            sort(pivot + 1, high);
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

    T front() { return datas[0]; }

    T back() { return datas[length]; }
};

Container<engine> engines;

// 电梯类
class elevator
{
public:
    elevator()
    {
        this->ID = ++ELEVATOR_NUMBER;
        engine temp;
        temp.ID = this->ID;
        engines.push_back(temp);
    }

    ~elevator() {}

    void move(int target) {}

    int get_floor() { return this->floor; }

    status get_status() { return this->STATUS; }

private:
    int ID, floor, station; // station 常驻楼层
    status STATUS = STATIC;
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
class numberDisplayerElement
{
public:
    numberDisplayerElement(
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

        this->reset_pin();
    }

    ~numberDisplayerElement() {}

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

    void display_number(int position, int number, int keepTime)
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

    void reset_pin()
    {
        for (int i = 1; i <= 4; i++)
            digitalWrite(this->pinLight[this->controlNumber[i]], LOW);
        for (int i = 1; i <= 8; i++)
            digitalWrite(this->pinLight[this->numberLights[i]], HIGH);
    }

    void update(int elevator1, int elevator2, int keepTime = 1)
    {
        this->display_point(2, keepTime);
        this->display_point(3, keepTime);
        this->display_number(1, elevator1 / 10, keepTime);
        this->display_number(2, elevator1 % 10, keepTime);
        this->display_number(3, elevator2 / 10, keepTime);
        this->display_number(4, elevator2 % 10, keepTime);
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

    numberData datas[10] = {
        numberData(1, 1, 1, 1, 1, 1, 0),
        numberData(0, 0, 1, 1, 0, 0, 0),
        numberData(0, 1, 1, 0, 1, 1, 1),
        numberData(0, 1, 1, 1, 1, 0, 1),
        numberData(1, 0, 1, 1, 0, 0, 1),
        numberData(1, 1, 0, 1, 1, 0, 1),
        numberData(1, 1, 0, 1, 1, 1, 1),
        numberData(0, 1, 1, 1, 0, 0, 0),
        numberData(1, 1, 1, 1, 1, 1, 1),
        numberData(1, 1, 1, 1, 1, 0, 1),
    };
};

numberDisplayerElement numberDisplayer(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

void setup()
{
    Serial.begin(9600);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
}

void loop()
{
    numberDisplayer.update(88, 88);
}
