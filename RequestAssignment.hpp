#ifndef SOLOMARK_AU_REQUESTASSIGNMENT
#define SOLOMARK_AU_REQUESTASSIGNMENT
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "jsoncpp/json/json.h"
#include "ElevatorDispatching.hpp"
using namespace std;
using namespace Json;

ifstream config;
Value root;
Reader reader;
long long n = -99999999, whole = 0;
const int maxService = 50;
int floorNumber, elevatorNumber, highOfFloor, elevatorSpeed;

struct requestData
{
    long long id;
    bool isNull = true;
    int idR = -1, idT = -1;
    int request = 0, target = 0;
    bool finiR = false, finiT = false;
    bool isUp = true;

    bool operator==(requestData q)
    {
        return request == q.request && target == q.target && finiR == q.finiR && finiT == q.finiT;
    }
};

requestData *reqPool[MAXFLOORNUM + 1][maxService];
requestData *nullReq;

void arrange_target(requestData *d) {};
void del_target(int request, int target) {};

inline bool isPoolEmpty();

inline void init_module(const char *configPath);

inline void add_target(int request, int target);

inline void register_target(requestData *d, int floor);

inline void reset_assignment(vector<dataSwaper> *data_);

inline int get_configData(ifstream *file, const char *key);

inline void reset_assignment(vector<dataSwaper> *data_)
{
    while (whole)
    {
        for (dataSwaper obj : *data_)
        {
            int cnt = 0;
            requestData *t;
            for (requestData *d : reqPool[obj.floor + obj.status_])
            {
                cnt++;
                if (d->isUp == obj.status_ && ((!d->finiR && obj.status_ > 0 ? d->request <= obj.maxF : d->request >= obj.minF && obj.status_ > 0 ? d->request > obj.floor : d->request < obj.floor) ||
                (!d->finiT && obj.status_ > 0 ? d->target <= obj.maxF : d->target >= obj.minF && obj.status_ > 0 ? d->target > obj.floor : d->target < obj.floor)))
                {
                    arrange_target(d);
                    t = d;
                    d = nullReq;
                    whole--;
                }
            }
            if (!cnt) {
                arrange_target(reqPool[obj.floor + obj.status_][0]);
                reqPool[obj.floor + obj.status_][0] = nullReq;
                whole--;
            }
            else {
                del_target(t->request, t->target);
                add_target(t->request, t->target);
            }
                
        }
    }
}

inline void init_module(const char *configPath)
{
    config.open(configPath);
    if (!config.is_open())
        throw "FileNotOpenedError: The target file is not opened.";
    if (!reader.parse(config, root, false))
        throw "FileDecodeFailureError: Decode the config file unsuccessfully.";
    FLOOR = floorNumber = get_configData(&config, "floorNumber");
    HighOfFloor = highOfFloor = get_configData(&config, "highOfFloor");
    Speed = elevatorSpeed = get_configData(&config, "elevatorSpeed");
    ELEVATORNUM = elevatorNumber = get_configData(&config, "elevatorNumber");
}

inline int get_configData(ifstream *file, const char *key)
{
    return root[key].asInt();
}

inline void add_target(int request, int target)
{
    if (request == target)
        return;
    requestData *t;
    t->request = request;
    t->target = target;
    t->id = ++n;
    t->isUp = target > request;
    register_target(t, request);
    register_target(t, target);
    whole++;
}

inline void register_target(requestData *d, int floor)
{
    bool isLive = false;
    for (int j = 0; j < maxService; j++)
    {
        if (*reqPool[floor][j] == *d)
        {
            isLive = true;
            break;
        }
    }
    if (!isLive)
    {
        for (int j = 0; j < maxService; j++)
            if (reqPool[floor][j]->isNull)
            {
                reqPool[floor][j] = d;
                if (reqPool[floor][j]->idR == -1)
                    reqPool[floor][j]->idR = j;
                else
                    reqPool[floor][j]->idT = j;
            }
    }
}

inline bool isPoolEmpty()
{
    for (int i = 1; i <= MAXFLOORNUM + 1; i++)
        for (int j = 0; j < maxService; j++)
            if (!reqPool[i][j]->isNull)
                return false;
    return true;
}
#endif