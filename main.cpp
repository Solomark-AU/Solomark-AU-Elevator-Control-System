#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "jsoncpp/json/json.h"
#include "Algorithm.hpp"
using namespace std;
using namespace Json;

ifstream config;
Value root;
Reader reader;
int floorNumber, elevatorNumber, highOfFloor, elevatorSpeed;

struct dataSwaper
{
    int floor, high;
    int *pointedTarget;
    STATUS status_;
    vector<REQUEST> *targets;
};

struct requestSpilt
{
    vector<int> *requests, *target;
};

inline void init_module(const char *configPath);

inline void reset_assignment(vector<dataSwaper> *data_);

inline requestSpilt *splitRequest(vector<REQUEST> *request);

inline int get_targetUsingTime(dataSwaper *data_);

inline int get_configData(ifstream *file, const char *key);

int main()
{
    init_module("configure.json");
    return 0;
}

inline void reset_assignment(vector<dataSwaper> *data_)
{
}

inline int get_targetUsingTime(dataSwaper *data_)
{
    int length = data_->targets->size();
    for (int i = 0; i < length; i++)
    {
    }
}

inline requestSpilt *splitRequest(vector<REQUEST> *request)
{
    requestSpilt *ans;
    for (int i = 0; i < request->size(); i++)
    {
        ans->requests->push_back((*request)[i].req);
        ans->target->push_back((*request)[i].tar);
    }
    return ans;
}

inline void init_module(const char *configPath)
{
    config.open(configPath);
    if (!config.is_open())
        throw "FileNotOpenedError: The target file is not opened.";
    if (!reader.parse(config, root, false))
        throw "FileDecodeFailureError: Decode the config file unsuccessfully.";
    floorNumber = get_configData(&config, "floorNumber");
    highOfFloor = get_configData(&config, "highOfFloor");
    elevatorSpeed = get_configData(&config, "elevatorSpeed");
    elevatorNumber = get_configData(&config, "elevatorNumber");
}

inline int get_configData(ifstream *file, const char *key)
{
    return root[key].asInt();
}