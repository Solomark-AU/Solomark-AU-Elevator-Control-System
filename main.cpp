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

inline int get_configData(ifstream *file, const char *key) { return root[key].asInt(); }

void init_module(const char *configPath)
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

struct dataSwaper
{
    int floor;
    STATUS status_;
    vector<REQUEST> *targets;
};



int main()
{
    init_module("configure.json");
    return 0;
}
