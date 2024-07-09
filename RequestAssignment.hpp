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
int floorNumber, elevatorNumber, highOfFloor, elevatorSpeed;

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

inline void reset_assignment(vector<dataSwaper> *data_);

inline int get_configData(ifstream *file, const char *key);

inline void reset_assignment(vector<dataSwaper> *data_)
{
    int DebugCounter = 0;
    while (whole > 0 && DebugCounter < 20)
    {
        DebugCounter++;
        cout << "RESET ASSIGNMENT: with the \"whole\"" << whole << "\n";
        int index = 0;
        debug_displayPtrSheetReqPool();
        for (dataSwaper obj : *data_)
        {
            int cnt = 0;
            requestData *t;
            for (requestData *d : reqPool[obj.floor + obj.status_])
            // for (int i = 0; i < 50; i++)
            {
                // requestData *d = reqPool[obj.floor + obj.status_][i];
                cnt++;
                if (d == NULL)
                    break;
                if (d->isUp == obj.status_ && ((!d->finiR && obj.status_ > 0 ? d->request <= obj.maxF : d->request >= obj.minF && obj.status_ > 0 ? d->request > obj.floor
                                                                                                                                                  : d->request < obj.floor) ||
                                               (!d->finiT && obj.status_ > 0 ? d->target <= obj.maxF : d->target >= obj.minF && obj.status_ > 0 ? d->target > obj.floor
                                                                                                                                                : d->target < obj.floor)))
                {
                    cout << "RESET ASSIGNMENT: ARCHOR THE REQUEST " << d->request << " @" << &d->request << "\n";
                    cout << "RESET ASSIGNMENT: ARCHOR THE TARGET " << reqPool[obj.floor + obj.status_][0]->target << " @" << &d->target << "\n";
                    Elist[index].add_target(d->request, d->target);
                    t = d;
                    d = nullReq;
                    whole--;
                }
                if (whole <= 0)
                    break;
            }
            if (reqPool[obj.floor + obj.status_][0] != NULL)
            {
                cout << "RESET ASSIGNMENT: ARCHOR THE REQUEST " << reqPool[obj.floor + obj.status_][0]->request << " @" << &reqPool[obj.floor + obj.status_][0]->request << "\n";
                cout << "RESET ASSIGNMENT: ARCHOR THE TARGET " << reqPool[obj.floor + obj.status_][0]->target << " @" << &reqPool[obj.floor + obj.status_][0]->target << "\n";
                if (cnt)
                {
                    Elist[index].add_target(reqPool[obj.floor + obj.status_][0]->request, reqPool[obj.floor + obj.status_][0]->target);
                    reqPool[obj.floor + obj.status_][0] = nullReq;
                    whole--;
                }
                else
                {
                    Elist[index].del_target(reqPool[obj.floor + obj.status_][0]->request, reqPool[obj.floor + obj.status_][0]->target);
                    add_target(t->request, t->target);
                }
            }
            index++;
            if (index == MAXELEVATORNUM)
                index = 0;
            if (whole <= 0)
                break;
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
    nullReq = &qwertyuiop;
    for (int i = 1; i <= MAXFLOORNUM; i++)
        for (int j = 0; j < maxService; j++)
            reqPool[i][j] = nullReq;
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
    whole++;
    vector<dataSwaper> temp;
    for (Elevator i : Elist)
    {
        dataSwaper c = i.GetData();
        cout << "LINK[ELEVATOR (" << &i << ") SWAP DATA @" << &c << "] {maxF:" << c.maxF << ", minF:" << c.minF << ", floor:" << c.floor << ", status:" << c.status_ << "}\n";
        temp.push_back(c);
        // cout << "Herobrine6265's DebugInfo at Line 143:" << reqPool[c.floor+c.status_]->request << endl;
    }
    reset_assignment(&temp);
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
