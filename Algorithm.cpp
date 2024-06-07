#include <iostream>
using namespace std;

const int MAXSERVERNUMBER = 4;
const int MAXCONTAINERNUMBER = 4;
int ELEVATOR_NUMBER = 0;

// status 电梯状态
// DOWNSIDE下行 STATIC不动 UPSIDE上行
enum status
{
    DOWNSIDE = -1,
    STATIC = 0,
    UPSIDE = 1
};

inline int abs(int num)
{
    return num >= 0 ? num : -num;
}

template <typename T>
inline void swap(T *src, T *val)
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

    T operator[](int index)
    {
        /*if (index >= length || index < 0)
            throw "IndexError: Out of the index.";*/
        return this->datas[index];
    }

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
        if (!this->target.empty())
        {
            if (this->floor == this->station)
            {
                this->STATUS = STATIC;
                this->isAvailable = true;
            }
        }
    }

    void open_door()
    {
        // 电机成分
        this->isOpen = false;
    }

    int get_floor() { return this->floor; }

    status get_status() { return this->STATUS; }

private:
    int ID, floor = 0, station; // station 常驻楼层
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

Container<elevator> elevators;
int inputNum[2];

void init()
{
    elevators.push_back(elevator());
    elevators.push_back(elevator());
}

void loop()
{
    int minDistance = 1024, minDistanceNum;
    for (int i = 0; i < elevators.size(); i++)
    {
        if (elevators[i].isAvailable == 1 || elevators[i].get_status() * (inputNum[0] - elevators[i].get_floor()) > 0)
        {
            if (abs(inputNum[0] - elevators[i].get_floor()) < minDistance)
            {
                minDistanceNum = i;
                minDistance = abs(inputNum[0] - elevators[i].get_floor());
            }
        }
    }
}

int main()
{
    init();
    while (true)
        loop();
    return 0;
}
