#ifndef SOLOMARK_AU_ALGORITHM
#define SOLOMARK_AU_ALGORITHM
#include "object.hpp"

std::vector<elevatorWell> wells;

void init_system()
{
    wells.push_back(elevatorWell());
    wells[0].register_elevator();
    wells[0].register_elevator();
    wells[0].register_elevator();
}
#endif
