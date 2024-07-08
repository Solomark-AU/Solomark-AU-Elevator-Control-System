#ifndef SOLOMARK_AU_ELEVATORDISPATCH
#define SOLOMARK_AU_ELEVATORDISPATCH
#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>
#include "jsoncpp/json/json.h"
using namespace std;
using namespace Json;

const int MAXFLOORNUM = 3000, MAXELEVATORNUM = 15;
int FLOOR, ELEVATORNUM, HighOfFloor, Speed;

int requestedNum[MAXFLOORNUM + 5][7][25][65]; // Count by minute and allows a inaccuracy of ±5 mins;
int station[25][65][MAXELEVATORNUM + 5][7];   //
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
    int status = -1; // status: -1->ReqFloor Not Responded  1->ReqFloor Responded   0->Back To Station
    bool operator==(REQUEST b)
    {
        return this->req == b.req && this->tar == b.tar && this->status == b.status;
    }
};

struct dataSwaper
{
    int maxF, minF, floor;
    STATUS status_;
    vector<REQUEST> *targets;
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
        this->target.push_back(REQUEST{req, tar, -1});
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

    void del_target(int req, int tar)
    {
        REQUEST temp = REQUEST{req, tar, -1};
        bool NotFound = 1;
        for (int k = 0; k < this->target.size(); k++)
        {
            if (this->target[k] == temp)
            {
                this->target.erase(this->target.begin() + k);
                k--;
                NotFound = 0;
            }
        }
        if (NotFound)
            return;
        if (this->target.empty())
        {
            this->isAvailable = true;
            time_t t;
            time(&t);
            tm *temp = localtime(&t);
            int k = station[temp->tm_hour][temp->tm_min][this->ID][temp->tm_wday];
            if (this->floor != k)
            {
                this->target.push_back(REQUEST{k, k, 0});
                if (k < this->floor)
                    this->status = STATUS::DOWNSIDE;
                else
                    this->status = STATUS::UPSIDE;
            }
            else
                this->status = STATUS::STATIC;
            return;
        }
        this->highest = 0;
        this->lowest = MAXFLOORNUM;
        for (REQUEST i : this->target)
        {
            if (i.status == 0)
                continue;
            if ((i.status == -1 && (i.req - this->floor) * this->status > 0))
            {
                this->highest = max(this->highest, i.req);
                this->lowest = min(this->lowest, i.req);
            }
            if ((i.tar - this->floor) * this->status > 0 && (i.status == 1 || (i.req - this->floor) * this->status > 0))
            {
                this->highest = max(this->highest, i.tar);
                this->lowest = min(this->lowest, i.tar);
            }
        }
    }

    void move()
    {
        bool Not_Opened = true;
        this->high += this->status * Speed;
        this->floor = this->high / HighOfFloor + 1;
        for (int k = 0; k < this->target.size(); k++)
        {
            REQUEST i = this->target[k];
            if (i.status == 1 && this->floor == i.tar)
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
                i.status = 1;
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
                    this->target.push_back(REQUEST{k, k, 0});
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

    inline dataSwaper GetData()
    {
        dataSwaper temp;
        temp.floor = this->floor;
        temp.maxF = this->highest;
        temp.minF = this->lowest;
        temp.status_ = this->status;
        temp.targets = &(this->target);
        return temp;
    }

    int GetAllTargetNumber()
    {
        int count = 0;
        bool vis[MAXFLOORNUM + 5];
        for (int i = 0; i <= MAXFLOORNUM; i++)
            vis[i] = false;
        for (REQUEST i : this->target)
        {
            if (!vis[i.req])
                count++;
            vis[i.req] = true;
            if (!vis[i.tar])
                count++;
            vis[i.tar] = true;
        }
        return count;
    }

    int GetVaildTargetNumber()
    {
        int count = 0;
        bool vis[MAXFLOORNUM + 5];
        for (int i = 0; i <= MAXFLOORNUM; i++)
            vis[i] = false;
        for (REQUEST i : this->target)
        {
            if (!vis[i.req] && i.status == -1)
                count++;
            if (!vis[i.tar] && i.status != 0)
                count++;
            vis[i.req] = vis[i.tar] = true;
        }
        return count;
    }

    int GetWayTargetNumber(int way) // way:1->UPSIDE     -1->DOWNSIDE
    {
        if (way != 1 && way != -1)
            return -1;
        int count = 0;
        bool vis[MAXFLOORNUM + 5];
        for (int i = 0; i <= MAXFLOORNUM; i++)
            vis[i] = false;
        for (REQUEST i : this->target)
        {
            if (i.status == 0)
                continue;
            if ((i.status == -1 && (i.req - this->floor) * way > 0) && !vis[i.req])
            {
                count++;
                vis[i.req] = 1;
            }
            if (((i.tar - this->floor) * way > 0 && (i.status == 1 || (i.req - this->floor) * way > 0)) && !vis[i.tar])
            {
                count++;
                vis[i.tar] = 1;
            }
        }
        return count;
    }

    inline int GetWayTargetNumber(STATUS way) // way:1->UPSIDE     -1->DOWNSIDE
    {
        if (way == STATUS::UPSIDE)
            return this->GetWayTargetNumber(1);
        else if (way == STATUS::DOWNSIDE)
            return this->GetWayTargetNumber(-1);
        return -1;
    }

    STATUS get_status() { return this->status; }

private:
    int floor = 1, high = 0, ID; // station 常驻楼层
    int highest, lowest;
    vector<REQUEST> target;
    STATUS status = STATUS::STATIC;
    bool isAvailable = true;
};
#endif