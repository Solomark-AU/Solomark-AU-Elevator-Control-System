#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

const int MAXFLOORNUM = 10000, HIGHOFFLOOR = 50, SPEED = 1;
int FLOOR = 15;

int requestedNum[MAXFLOORNUM][7][25][65]; // Count by minute and allows a inaccuracy of ±5 mins;
int OldStation[7], LastUpDate = 0;

enum STATUS
{
    UPSIDE = 1,
    STATIC = 0,
    DOWNSIDE = -1
};

struct REQUEST
{
    int req, tar;
};

class Elevator
{
public:
    void add_target(int req, int tar)
    {
        time_t t;
        time(&t);
        struct tm *time_info = localtime(&t);
        if (time_info->tm_wday != LastUpDate)
        {
            for (int i = 0; i < 25; i++)
                for (int j = 0; j < 61; j++)
                {
                    for (int k = 1; k < MAXFLOORNUM; k++)
                        requestedNum[k][time_info->tm_wday][i][j] = 0;
                    requestedNum[OldStation[LastUpDate]][LastUpDate][i][j] = 100;
                    requestedNum[FLOOR / 2][LastUpDate][i][j] = 30;
                }
            LastUpDate = time_info->tm_wday;
        }
        for (int i = time_info->tm_min - 5; i <= time_info->tm_min + 5; t++)
            requestedNum[req][LastUpDate][time_info->tm_hour + (60 + i) / 60 - 1][(60 + i) % 60]++;
        // push into vector
        this->target.push_back(REQUEST{req, tar});
        if ((req - this->floor) * this->status > 0)
        {
            this->highest = max(highest, req);
            this->floor = max(highest, req);
        }
        if (this->isAvailable)
        {
            this->highest = req;
            this->lowest = req;
            if (this->floor - req > 0)
                this->status = STATUS::UPSIDE;
            else if (this->floor - req > 0)
                this->status = STATUS::DOWNSIDE;
            else
                this->status = STATUS::STATIC;
        }

        this->isAvailable = false;
    }

    void move()
    {
        bool Not_Opened = 1;
        this->high += this->status * SPEED;
        this->floor = this->high / HIGHOFFLOOR + 1;
        for (int k = 0; k < this->target.size(); k++)
        {
            REQUEST i = this->target[k];
            if (i.req == this->floor)
            {
                i.req = i.tar;
                if (Not_Opened)
                {
                    this->open_door();
                    Not_Opened = false;
                }
            }
            if (i.req == i.tar && this->floor == i.tar)
            {
                this->target.erase(this->target.begin() + k);
                k--;
                if (Not_Opened)
                {
                    this->open_door();
                    Not_Opened = false;
                }
            }
        }
        //Change the Way
    }

    void open_door() {}

    int get_floor() { return this->floor; }

    int get_targetNumber() { return this->target.size(); }

    STATUS get_status() { return this->status; }

private:
    int floor = 1, station = 1, high = 0; // station 常驻楼层
    int highest, lowest;
    vector<REQUEST> target;
    STATUS status = STATUS::STATIC;
    bool isAvailable = true;
};

int main()
{
    cin >> FLOOR;
    while (true)
    {
        int request, target;
        cin >> request >> target;
    }
    return 0;
}