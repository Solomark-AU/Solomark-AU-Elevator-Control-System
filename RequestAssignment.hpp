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
long long n = -999999999, whole = 0;
const int maxService = 5;

Elevator Elist[MAXELEVATORNUM];

struct requestData
{
    long long id = 0;
    bool isNull = true;
    int idR = -1, idT = -1;
    int request = 0, target = 0;
    bool finiR = false, finiT = false;
    bool isUp = true;
};

requestData *reqPool[MAXFLOORNUM + 1][maxService];

requestData qwertyuiop;
requestData *nullReq;

inline bool isPoolEmpty();

inline void debug_displayPtrSheetReqPool(int sF = 1, int eF = MAXFLOORNUM, int sI = 0, int eI = maxService);

inline void init_module(const char *configPath);

inline void add_target(int request, int target);

inline void register_target(requestData *d, int floor);

inline int get_configData(ifstream *file, const char *key);

inline void init_module(const char *configPath)
{
    config.open(configPath);
    if (!config.is_open())
        throw "FileNotOpenedError: The target file is not opened.";
    if (!reader.parse(config, root, false))
        throw "FileDecodeFailureError: Decode the config file unsuccessfully.";
    FLOOR = get_configData(&config, "floorNumber");
    HighOfFloor = get_configData(&config, "highOfFloor");
    Speed = get_configData(&config, "elevatorSpeed");
    ELEVATORNUM = get_configData(&config, "elevatorNumber");
    OpendoorTime = get_configData(&config, "opendoorTime");
    nullReq = &qwertyuiop;
    for (int i = 1; i <= MAXFLOORNUM; i++)
        for (int j = 0; j < maxService; j++)
            reqPool[i][j] = nullReq;
    for (int i = 0; i < MAXELEVATORNUM; i++)
        Elist[i].ElevatorInit(i);
}

inline int get_configData(ifstream *file, const char *key)
{
    return root[key].asInt();
}

inline void add_target(int request, int target)
{
    if (request == target)
        return;
    requestData t;
    t.request = request;
    t.target = target;
    t.id = ++n;
    t.isUp = target > request;
    register_target(&t, request);
    register_target(&t, target);
    ResTime minResTime = Elist[0].RespondTime(request, target);
    for (int i = 1; i < ELEVATORNUM; i++)
    {
        /*
        if (Elist[minId].DistanceTime(request, target - request > 0) > Elist[i].DistanceTime(request, target - request > 0))
            minId = i;
        if (Elist[minId].DistanceTime(request, target - request > 0) == Elist[i].DistanceTime(request, target - request > 0))
            if (Elist[minId].GetTargetSize() > Elist[i].GetTargetSize())
                minId = i;
        */
        ResTime temp = Elist[i].RespondTime(request, target);
        if (minResTime.TarTime > temp.TarTime)
            minResTime = temp;
        else if (minResTime.TarTime == temp.TarTime)
        {
            if (minResTime.ReqTime1 > temp.ReqTime1)
                minResTime = temp;
            else if (minResTime.ReqTime1 == temp.ReqTime1)
                if (Elist[minResTime.ElevatorID].GetTargetSize() > Elist[i].GetTargetSize())
                    minResTime = temp;
        }
    }
    Elist[minResTime.ElevatorID].add_target(request, target);
}

inline void register_target(requestData *d, int floor)
{
    cout << "FUNCTION[REGISTER REQUEST with 0]\n";
    bool isLive = false;
    for (int j = 0; j < maxService; j++)
    {
        if (reqPool[floor][j] == NULL)
            break;
        if (reqPool[floor][j]->request == d->request &&
            reqPool[floor][j]->target == d->target &&
            reqPool[floor][j]->finiR == d->finiR &&
            reqPool[floor][j]->finiT == d->finiT)
        {
            isLive = true;
            cout << "REGISTER REQUEST: Has the same, in the garbage (" << d->request << ", " << d->target << ")\n";
            cout << "QUIT[REGISTER REQUEST with -1]\n";
            break;
        }
    }
    if (!isLive)
    {
        for (int j = 0; j < maxService; j++)
        {
            if (reqPool[floor][j] == NULL)
            {
                cout << "LINK[REQUEST POOL with NULLPTR]\n";
                continue;
            }
            if (reqPool[floor][j] == nullReq)
            {
                reqPool[floor][j] = d;
                if (reqPool[floor][j]->idR == -1)
                    reqPool[floor][j]->idR = j;
                else
                    reqPool[floor][j]->idT = j;
                cout << "REGISTER REQUEST: REGISTER AT " << reqPool[floor][j] << " (" << d->request << ", " << d->target << ")\n";
                cout << "QUIT[REGISTER REQUEST with 0]\n";
                return;
            }
        }
        cout << "QUIT[REGISTER REQUEST with NONE OPERATION]\n";
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

inline void debug_displayPtrSheetReqPool(int sF, int eF, int sI, int eI)
{
    for (int i = sF; i <= eF; i++)
        for (int j = sI; j < eI; j++)
            cout << "DEBUG::DISPLAY[REQUEST POOL](" << i << ", " << j << ") @ " << reqPool[i][j] << "\n";
}

#endif
