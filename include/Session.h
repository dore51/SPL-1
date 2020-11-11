#ifndef SESSION_H_
#define SESSION_H_
#include <vector>
#include <string>
#include "Graph.h"

class Tree;
class Agent;

using namespace std;

enum TreeType{
  Cycle,
  MaxRank,
  Root
};

class Session{
public:
    // rule of 5
    Session(const std::string& path);
    Session(const Session&);
    virtual ~Session();
    Session& operator=(const Session &other);
    Session(Session &&other);
    Session& operator=(Session &&other);
    // functions
    void simulate();
    void addAgent(const Agent& agent);
    void setGraph(const Graph& graph);
    void enqueueInfected(int);
    int dequeueInfected();
    int getCycle() const;
    void nodeDisconnect(int i);
    void infectNode(const int i);
    int findHealthyNeighbor(const int i);
    void fullyInfectNode(const int i);
    TreeType getTreeType() const;
    Tree *createBFS(int node);

private:
    Graph g;
    int cycle;
    TreeType treeType;
    std::vector<Agent*> agents;
    std::vector<int> infectedQueue;
    unsigned int nextDequeue;
    Agent *makeAgent(const std::string& type, int index);
    bool Pandemic();
    void setTreeType(const string& type);
    void clear();
    void steal(Session &other);
    void copy(const Session &other);
};

#endif
