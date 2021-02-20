#include <iostream>
#include "Session.h"

using namespace std;

int main(int argc, char** argv){
    if(argc != 2){
        cout << "usage cTrace <config_path>" << endl;
        return 0;
    }
    Session* sessPtr = new Session(argv[1]);
    Session copy(*sessPtr);
    delete(sessPtr);
    copy.simulate();
    return 0;
}
