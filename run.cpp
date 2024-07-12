#include <iostream>
using namespace std;
int main()
{
    system("g++ main.cpp -o main -fsanitize=undefined -L/usr/local/lib jsoncpp/libjsoncpp.so.1.9.5");
    cout << "Compile Finish!\n\n\n";
    system("\"/home/shanyu/桌面/Solomark-AU-Elevator-Control-System/\"main");
    return 0;
}