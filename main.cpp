#include "RequestAssignment.hpp"
#include "ElevatorDispatching.hpp"

int main()
{
    init_module("configure.json");
    while (1)
    {
        int req, tar;
        cin >> req >> tar;
        cout << "F**k zy\n";
        add_target(req, tar);
        cout << "F**k RBQ\n";
        for (int i = 0; i < ELEVATORNUM; i++)
        {
            Elist[i].move();
            dataSwaper temp = Elist[i].GetData();
            cout << "Elevator[" << i << "]: Status:" << temp.status_ << " Floor:" << temp.floor << endl;
        }
        time_t t1, t2;
        time(&t1);
        while (t2 - t1 < 1)
            time(&t2);
    }
    return 0;
}
