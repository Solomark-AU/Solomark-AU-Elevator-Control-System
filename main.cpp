#include "object.hpp"
using namespace IO;

std::vector<elevatorWell> wells;

void init_system();

int main()
{
    init_system();
    return 0;
}

void init_system()
{
    wells.push_back(elevatorWell());
    wells[0].register_elevator();
    wells[0].register_elevator();
    wells[0].register_elevator();
}
