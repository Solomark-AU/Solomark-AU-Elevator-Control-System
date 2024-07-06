#ifndef SOLOMARK_AU_ALGORITHM
#define SOLOMARK_AU_ALGORITHM
#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>
#include "jsoncpp/json/json.h"
using namespace std;
using namespace Json;

const int MAXFLOORNUM = 10000, MAXELEVATORNUM = 10;
int FLOOR, ELEVATORNUM, HighOfFloor, Speed;

int requestedNum[MAXFLOORNUM][7][25][65]; // Count by minute and allows a inaccuracy of ±5 mins;
int station[25][65][MAXELEVATORNUM][7];   //
int LastUpDate = -1;

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

bool cmp_bigger(int a, int b)
{
    return a > b;
}

class Elevator
{
public:
    Elevator(int id) { this->ID = id; }

    ~Elevator() {}

    void add_target(int req, int tar)
    {
        time_t t;
        time(&t);
        tm *time_info = localtime(&t);
        if (time_info->tm_wday != LastUpDate)
        {
            for (int i = 0; i < 25; i++)
                for (int j = 0; j < 61; j++)
                {
                    vector<int> reqNum;
                    for (int k = 1; k < MAXFLOORNUM; k++)
                    {
                        reqNum.push_back(requestedNum[k][time_info->tm_wday][i][j]);
                        requestedNum[k][time_info->tm_wday][i][j] = 0;
                    }
                    sort(reqNum.begin(), reqNum.end(), cmp_bigger);
                    for (int k = 1; k <= ELEVATORNUM && k <= 10; k++)
                    {
                        requestedNum[station[i][j][k][time_info->tm_wday]][time_info->tm_wday][i][j] += 11 - k;
                        station[i][j][k][time_info->tm_wday] = reqNum[k - 1];
                    }
                    requestedNum[FLOOR / 2][time_info->tm_wday][i][j] += 3;
                }
            LastUpDate = time_info->tm_wday;
        }
        for (int i = -5; i <= 5; t++)
        {
            t -= i * 60;
            tm *temp = localtime(&t);
            requestedNum[req][temp->tm_wday][temp->tm_hour][temp->tm_min]++;
            vector<int> reqNum;
            for (int k = 1; k < MAXFLOORNUM; k++)
                reqNum.push_back(requestedNum[k][temp->tm_wday][temp->tm_hour][temp->tm_min]);
            sort(reqNum.begin(), reqNum.end(), cmp_bigger);
            for (int k = 1; k <= ELEVATORNUM && k <= 10; k++)
                station[temp->tm_hour][temp->tm_min][k][time_info->tm_wday] = reqNum[k - 1];
            t += i * 60;
        }
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
        bool Not_Opened = true;
        this->high += this->status * Speed;
        this->floor = this->high / HighOfFloor + 1;
        for (int k = 0; k < this->target.size(); k++)
        {
            REQUEST i = this->target[k];
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
            if (i.req == this->floor)
            {
                i.req = i.tar;
                if (Not_Opened)
                {
                    this->open_door();
                    Not_Opened = false;
                }
            }
        }
        // Change the Way
        if ((this->floor == this->lowest && this->status == STATUS::DOWNSIDE) ||
            (this->floor == this->highest && this->status == STATUS::UPSIDE))
        {
            this->highest = 0;
            this->lowest = MAXFLOORNUM;
            for (REQUEST i : this->target)
            {
                this->highest = max(this->highest, max(i.req, i.tar));
                this->lowest = min(this->lowest, min(i.req, i.tar));
            }
            if (this->status == STATUS::UPSIDE)
                this->status == STATUS::DOWNSIDE;
            else if (this->status == STATUS::DOWNSIDE)
                this->status == STATUS::UPSIDE;
            if (this->target.empty())
            {
                this->isAvailable = true;
                time_t t;
                time(&t);
                tm *temp = localtime(&t);
                int k = station[temp->tm_hour][temp->tm_min][this->ID][temp->tm_wday];
                if (this->floor != k)
                {
                    this->target.push_back(REQUEST{k, k});
                    if (k < this->floor)
                        this->status = STATUS::DOWNSIDE;
                    else
                        this->status = STATUS::UPSIDE;
                }
                else
                    this->status = STATUS::STATIC;
            }
        }
    }

    void open_door() {}

    int get_floor() { return this->floor; }

    int get_targetNumber() { return this->target.size(); }

    STATUS get_status() { return this->status; }

private:
    int floor = 1, high = 0, ID; // station 常驻楼层
    int highest, lowest;
    vector<REQUEST> target;
    STATUS status = STATUS::STATIC;
    bool isAvailable = true;
};
#endif