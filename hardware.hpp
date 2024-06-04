#ifndef SOLOMARK_AU_HARDWARE
#define SOLOMARK_AU_HARDWARE
#include <vector>

enum status
{
    DOWNSIDE = -1,
    STATIC = 0,
    UPSIDE = 1
};

struct engine
{
    int ID;
    status STATUS = STATIC;
};

std::vector<engine> engines;

#endif