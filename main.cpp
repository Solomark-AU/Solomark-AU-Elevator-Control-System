#include "RequestAssignment.hpp"
#include "ElevatorDispatching.hpp"



int main()
{
    init_module("configure.json");
    while (1)
    {
        int req, tar;
        cin >> req >> tar;
        cout<<"Fuck zy\n";
        add_target(req, tar);
        cout<<"Fuck RBQ\n";
    }
    return 0;
}
