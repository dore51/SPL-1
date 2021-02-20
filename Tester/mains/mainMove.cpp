#include <iostream>
#include "Session.h"

using namespace std;

int main(int argc, char** argv){
    if(argc != 2){
        cout << "usage cTrace <config_path>" << endl;
        return 0;
    }
    Session orig(argv[1]);
    Session copy = std::move(orig);
    copy.simulate();
    return 0;
}
