#ifndef SOLOMARK_AU_OBJECT
#define SOLOMARK_AU_OBJECT

#include <iostream>
#include <vector>
#include "hardware.hpp"

int ELEVATOR_NUMBER = 0;

namespace IO
{
    int input()
    {
        int n;
        std::cin >> n;
        return n;
    }

    void print(int n, bool ent = 0)
    {
        std::cout << n;
        if (ent)
            std::cout << std::endl;
    }
    void print(std::string s, bool ent = 0)
    {
        std::cout << s;
        if (ent)
            std::cout << std::endl;
    }
};

class elevator
{
public:
    elevator()
    {
        this->ID = ++ELEVATOR_NUMBER;
        engines.push_back(engine{this->ID});
        std::cout << "[System]<Init> init an elevator successfully\n";
    }

    ~elevator() {}

    void move() {}

    int get_floor() { return this->floor; }

    status get_status() { return this->STATUS; }

private:
    int ID, floor, station;
    status STATUS = STATIC;
};

class elevatorWell
{
public:
    elevatorWell() { 
        register_elevator(); 
        std::cout << "[System]<Init> init an elevator well successfully\n";
    }
    ~elevatorWell() {}

    int get_elevatorNumbers() { return WellNumber; };

    void register_elevator()
    {
        WellNumber++;
        this->group.push_back(elevator());
        std::cout << "[Elevator Well]<Register> register an elevator successfully\n";
    };

private:
    int WellNumber = 1;
    std::vector<elevator> group;
};
#endif