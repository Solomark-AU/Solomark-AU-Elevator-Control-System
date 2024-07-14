#include <iostream>
using namespace std;
int main()
{
    system("g++ main.cpp -o main -fsanitize=undefined,address -L/usr/local/lib jsoncpp/libjsoncpp.so.1.9.5");
    cout << "Compile Finish!\n\n\n";
    system("./main");
    return 0;
}