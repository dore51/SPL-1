#include <iostream>
#include <fstream>
#include "Tree.h"
#include "json.hpp"

#define JSON_PATH "output.json"

using namespace std;

int tree_exp1(){
    MaxRankTree t(1);
    MaxRankTree t2(2);
    t2.addChild(MaxRankTree(3));
    t2.addChild(MaxRankTree(4));
    t2.addChild(MaxRankTree(5));
    t.addChild(t2);
    t.addChild(MaxRankTree(6));
    return t.traceTree();
}

int tree_exp2(){
    RootTree t(1);
    t.addChild(RootTree(2));
    t.addChild(RootTree(3));
    t.addChild(RootTree(4));
    return t.traceTree();
}

int tree_exp3(){
    CycleTree t(1, 1);
    t.addChild(CycleTree(2, 1));
    t.addChild(CycleTree(3, 1));
    t.addChild(CycleTree(4, 1));
    return t.traceTree();
}


int main(int argc, char** argv){
    nlohmann::json j;
    j["tree1"] = tree_exp1();
    j["tree2"] = tree_exp2();
    j["tree3"] = tree_exp2();
    std::ofstream o(JSON_PATH);
    o << j << endl;
    return 0;
}
