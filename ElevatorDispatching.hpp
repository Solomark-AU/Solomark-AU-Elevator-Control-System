#ifndef SOLOMARK_AU_ELEVATORDISPATCH
#define SOLOMARK_AU_ELEVATORDISPATCH
#include <ctime>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "jsoncpp/json/json.h"
using namespace std;
using namespace Json;

const int MAXFLOORNUM = 10, MAXELEVATORNUM = 15;
int FLOOR, ELEVATORNUM = 0, HighOfFloor, Speed, OpendoorTime;

bool visID[MAXELEVATORNUM + 5];
int requestedNum[8][25][65][MAXFLOORNUM + 5]; // Count by minute and allows a inaccuracy of ±5 mins;
int station[8][25][65][MAXELEVATORNUM + 5];   //
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

struct ResTime
{
    // bool status; // True:ResTime1!=ResTime2     False:ResTime1==ResTime2
    int ReqTime1, /*ReqTime2,*/ TarTime, ElevatorID;
};

struct StationInfo
{
    int ReqNum, StationFloor;
};

bool cmp_bigger(StationInfo a, StationInfo b)
{
    return a.ReqNum > b.ReqNum;
}

string ntos(int n)
{
    string s = "";
    if (n == 0)
        return "0";
    while (n)
    {
        s = char(n % 10) + s;
        n /= 10;
    }
    return s;
}

class Elevator
{
public:
    Elevator() {}

    ~Elevator() {}

    inline void ElevatorInit(int id)
    {
        if (visID[id])
            throw "F**k U Link!You have used this ID!";
        this->ID = id;
        for (int k = 0; k <= 7; k++)
            for (int i = 0; i < 24; i++)
                for (int j = 0; j < 60; j++)
                    station[k][i][j][this->ID] = (FLOOR / ELEVATORNUM - 1) * id + 1;
        if (LastUpDate == -1)
        {
            time_t t;
            time(&t);
            LastUpDate = localtime(&t)->tm_wday;
        }
        this->inited = true;
    }

    inline bool arrive(int Floor) { return Floor == this->floor && this->high % HighOfFloor <= HighOfFloor / 2000; }

    inline bool arrive(int Floor, int High) { return Floor == (High / HighOfFloor + 1) && (High % HighOfFloor) * 2000 <= HighOfFloor; }

    void add_target(int req, int tar)
    {
        cout << "[LINK]Elevator::add_target():Get the data {" << req << ", " << tar << "},\n";
        time_t t;
        time(&t);
        tm *time_info = localtime(&t);
        if (time_info->tm_wday != LastUpDate)
        {
            for (int i = 0; i < 24; i++)
            {
                for (int j = 0; j < 60; j++)
                {
                    vector<StationInfo> StInfo;
                    bool vis[FLOOR + 5];
                    int minvis = 1;
                    for (int f = 1; f <= FLOOR; f++)
                    {
                        vis[f] = false;
                        StInfo.push_back(StationInfo{requestedNum[time_info->tm_wday][i][j][f], f});
                        requestedNum[time_info->tm_wday][i][j][f] = 0;
                    }
                    sort(StInfo.begin(), StInfo.end(), cmp_bigger);
                    for (int e = 0; e < ELEVATORNUM; e++)
                    {
                        if (StInfo[e].ReqNum)
                        {
                            vis[StInfo[e].StationFloor] = true;
                            station[time_info->tm_wday][i][j][e] = StInfo[e].StationFloor;
                        }
                        else
                        {
                            if (!vis[FLOOR / 2])
                            {
                                vis[FLOOR / 2] = 1;
                                station[time_info->tm_wday][i][j][e] = FLOOR / 2;
                            }
                            else if (!vis[1])
                            {
                                vis[1] = 1;
                                station[time_info->tm_wday][i][j][e] = 1;
                            }
                            else if (!vis[FLOOR])
                            {
                                vis[FLOOR] = 1;
                                station[time_info->tm_wday][i][j][e] = FLOOR;
                            }
                            else
                            {
                                while (1)
                                {
                                    if (!vis[minvis])
                                        station[time_info->tm_wday][i][j][e] = minvis;
                                    minvis++;
                                }
                            }
                        }
                        requestedNum[time_info->tm_wday][i][j][station[time_info->tm_wday][i][j][e]] += 2 * ELEVATORNUM - e;
                    }
                    requestedNum[time_info->tm_wday][i][j][FLOOR / 2] += 5;
                }
            }
            LastUpDate = time_info->tm_wday;
        }
        for (int i = -5; i <= 5; i++)
        {
            t += i * 60;
            time_info = localtime(&t);
            requestedNum[time_info->tm_wday][time_info->tm_hour][time_info->tm_min][req]++;
            t -= i * 60;
        }
        if (this->isAvailable)
        {
            this->highest = req;
            this->lowest = req;
            if (this->floor - req < 0)
                this->status = STATUS::UPSIDE;
            else if (this->floor - req > 0)
                this->status = STATUS::DOWNSIDE;
            else
            {
                if (this->high % HighOfFloor <= HighOfFloor / 2000)
                {
                    if (tar - req > 0)
                        this->status = STATUS::UPSIDE;
                    else if (tar - req < 0)
                        this->status = STATUS::DOWNSIDE;
                }
                else if (this->high < (req - 1) * HighOfFloor)
                    this->status = STATUS::UPSIDE;
                else
                    this->status = STATUS::DOWNSIDE;
            }
        }
        if (this->arrive(req))
        {
            this->target.push_back(REQUEST{req, tar, 1});
            this->open_door();
            if ((tar - this->floor) * this->status > 0)
            {
                this->highest = max(highest, tar);
                this->lowest = min(lowest, tar);
            }
        }
        else
        {
            this->target.push_back(REQUEST{req, tar, -1});
            if ((req - this->floor) * this->status > 0)
            {
                this->highest = max(highest, req);
                this->lowest = min(lowest, req);
            }
            if ((tar - this->floor) * this->status > 0 && (tar - req) * this->status > 0)
            {
                this->highest = max(highest, tar);
                this->lowest = min(lowest, tar);
            }
        }
        this->isAvailable = false;
    }

    void del_target(int req, int tar, int status = -1)
    {
        REQUEST temp = REQUEST{req, tar, status};
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
            if (!arrive(k))
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
            {
                this->highest = this->lowest = i.req;
                break;
            }
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

    void del_target(REQUEST temp_req) { this->del_target(temp_req.req, temp_req.tar, temp_req.status); }

    void move()
    {
        bool Not_Opened = true;
        this->high += this->status * Speed;
        this->floor = this->high / HighOfFloor + 1;
        for (int k = 0; k < this->target.size(); k++)
        {
            REQUEST i = this->target[k];
            if (i.status == 1 && this->arrive(i.tar))
            {
                this->del_target(i);
                k--;
                if (Not_Opened)
                {
                    this->open_door();
                    Not_Opened = false;
                }
            }
            if (i.status == -1 && this->arrive(i.req))
            {
                this->target[k].status = 1;
                if (Not_Opened)
                {
                    this->open_door();
                    Not_Opened = false;
                }
            }
        }
        // Change the Way
        if ((this->arrive(this->lowest) && this->status == STATUS::DOWNSIDE) ||
            (this->arrive(this->highest) && this->status == STATUS::UPSIDE))
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
                int k = station[temp->tm_wday][temp->tm_hour][temp->tm_min][this->ID];
                if (!this->arrive(k))
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

    void open_door()
    {
        time_t t1, t2;
        time(&t1);
        while (t2 - t1 < OpendoorTime)
            time(&t2);
    }

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

    inline int GetTargetSize() { return this->target.size(); }

    inline ResTime RespondTime(int ReqFloor, int TarFloor)
    {
        ResTime TimeTemp;
        TimeTemp.ElevatorID = this->ID;
        if (this->isAvailable)
        {
            TimeTemp.ReqTime1 = abs(this->high - (ReqFloor - 1) * HighOfFloor) / Speed;
            TimeTemp.TarTime = TimeTemp.ReqTime1 + OpendoorTime + abs(ReqFloor - TarFloor) * HighOfFloor / Speed;
            return TimeTemp;
        }
        vector<REQUEST> TargetTemp(this->target);
        int HighTemp = this->high, HighestTemp = this->highest, LowestTemp = this->lowest, NotVisReq = 1;
        STATUS StatusTemp = this->status;
        while (HighTemp < 500)
        {
            // cout << "Elevator[" << this->ID << "]::RespondTime -> HighTemp, StatusTemp: " << HighTemp << " " << StatusTemp << "\n";
            TimeTemp.ReqTime1 += NotVisReq;
            TimeTemp.TarTime++;
            HighTemp += StatusTemp * Speed;
            if (this->arrive(ReqFloor, HighTemp) && NotVisReq)
            {
                NotVisReq = 0;
                TimeTemp.TarTime += OpendoorTime;
            }
            if (this->arrive(TarFloor, HighTemp) && !NotVisReq)
                break;
            for (int k = 0; k < TargetTemp.size(); k++)
            {
                REQUEST i = TargetTemp[k];
                if (i.status == 1 && this->arrive(i.tar, HighTemp))
                {
                    TargetTemp.erase(TargetTemp.begin() + k);
                    k--;
                    TimeTemp.ReqTime1 += NotVisReq * OpendoorTime;
                    TimeTemp.TarTime += OpendoorTime;
                }
                if (i.status == -1 && this->arrive(i.req, HighTemp))
                {
                    TargetTemp[k].status = 1;
                    TimeTemp.ReqTime1 += NotVisReq * OpendoorTime;
                    TimeTemp.TarTime += OpendoorTime;
                }
            }
            // Change the Way
            if ((this->arrive(LowestTemp, HighTemp) && StatusTemp == STATUS::DOWNSIDE) ||
                (this->arrive(HighestTemp, HighTemp) && StatusTemp == STATUS::UPSIDE))
            {
                HighestTemp = 0;
                LowestTemp = MAXFLOORNUM;
                for (REQUEST i : TargetTemp)
                {
                    HighestTemp = max(HighestTemp, max(i.req, i.tar));
                    LowestTemp = min(LowestTemp, min(i.req, i.tar));
                }
                if (StatusTemp == STATUS::UPSIDE)
                    StatusTemp == STATUS::DOWNSIDE;
                else if (StatusTemp == STATUS::DOWNSIDE)
                    StatusTemp == STATUS::UPSIDE;
                if (TargetTemp.empty())
                {
                    cout << "TargetTemp.empty()";
                    if (NotVisReq)
                    {
                        TimeTemp.ReqTime1 += abs(HighTemp - (ReqFloor - 1) * HighOfFloor) / Speed;
                        HighTemp = (ReqFloor - 1) * HighOfFloor;
                    }
                    TimeTemp.TarTime += abs(HighTemp - (TarFloor - 1) * HighOfFloor) / Speed;
                    break;
                }
            }
        }
        return TimeTemp;
    }

private:
    int floor = 1, high = 0, ID; // ID:start from 0
    int highest, lowest;
    vector<REQUEST> target;
    STATUS status = STATUS::STATIC;
    bool isAvailable = true, inited = false;
};
#endif