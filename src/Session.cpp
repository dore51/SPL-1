#include <iostream>
#include "../include/Session.h"
#include "../include/Agent.h"
#include <fstream>
#include "../include/json.hpp"
using namespace std;
using json = nlohmann::json;

// constructor
Session::Session(const string &path): g(vector<vector<int>>()), cycle(0), treeType(), agents(vector<Agent*>()), infectedQueue(vector<int>()), nextDequeue(0) {
    ifstream i(path);
    json j;
    i >> j;
    setGraph( Graph(j["graph"]));
    for (auto &elem: j["agents"]) {
        Agent* agent = makeAgent(elem[0],elem[1]);
        if (agent)
            agents.push_back(agent);
    }
    setTreeType(j["tree"]);

}

// copy constructor
Session::Session(const Session &other) : g(other.g), cycle(other.cycle), treeType(other.treeType), agents(vector<Agent*>()), infectedQueue(other.infectedQueue), nextDequeue(other.nextDequeue){
    for (unsigned int index = 0;index<other.agents.size();index++) {
        Agent* newAgent = other.agents[index]->clone();
        agents.push_back(newAgent);
    }
}

// destructor
Session::~Session() { clear(); }

// copy assignment operator
Session& Session::operator=(const Session &other) {
    if (this != &other) {
        clear(); // this and other will never share same resources
        copy(other);
    }
       return *this;
}

// move constructor
Session::Session(Session &&other):g(other.g), cycle(other.cycle), treeType(other.treeType), agents(vector<Agent*>()), infectedQueue(other.infectedQueue), nextDequeue(other.nextDequeue){
    steal(other);
}


// move assignment
Session& Session::operator=(Session &&other) { // if need temp
    if (this != &other) {
        clear();
        // on stack values
        g = other.g;
        cycle = other.cycle;
        treeType = other.treeType;
        infectedQueue = other.infectedQueue;
        nextDequeue = other.nextDequeue;
        steal(other);
    }
    return *this;
}

// steal all other's resources
void Session::steal(Session &other) {
    agents = vector<Agent*>();
    for (unsigned int index = 0;index<other.agents.size();index++) {
        Agent *newAgent = (other.agents[index]);
        agents.push_back(newAgent);
        other.agents[index] = nullptr;
    }
    other.agents.clear();
}

// create a copy of other (on heap)
void Session::copy(const Session &other) {
    g = other.g;
    cycle = other.cycle;
    treeType = other.treeType;
    agents = vector<Agent*>();
    infectedQueue = other.infectedQueue;
    nextDequeue = other.nextDequeue;
    for (unsigned int index = 0;index<other.agents.size();index++) {
        Agent *newAgent = (other.agents[index]->clone());
        agents.push_back(newAgent);
    }
}

// delete resources
void Session::clear() {
    for (Agent* agent : agents)
        if (agent) {
            delete agent;
            agent = nullptr;
        }
    agents.clear();
}

// simulate that we save the world from the pandemic (caused by the chinese government, lead by Ms.chang)
void Session::simulate() {
    while (Pandemic()){
        unsigned int agentsSize = agents.size();
        for (unsigned int i = 0; i < agentsSize; i++){
            agents[i]->act(*this);
        }
        cycle++;
    }
    json j;
    j["graph"] = g.getMatrix();
    j["infected"] = g.getInfectedNodes();
    ofstream i("./output.json");
    i << j;

}
// returns true if the pandemic still going home
bool  Session::Pandemic (){
    return g.isThereCarrierNotActed(); // !(all components are either fully infected or virus-free) = there's a carrier that not acted yet
}


int Session::getCycle() const {
    return cycle;
}

TreeType Session::getTreeType() const {
    return treeType;
}

// enqueue a new virus to the queue of which the contact tracer act's on
void Session::enqueueInfected(int node) {
    infectedQueue.push_back(node);
}

// dequeue a virus from the queue of which the contact tracer act's on
int Session::dequeueInfected() {
    if (nextDequeue < infectedQueue.size()) {
        int nodeInd = infectedQueue[nextDequeue];
        nextDequeue++;
        return nodeInd;
    }
    return -1; // if there isn't a virus in the queue
}

// adds a new agent to the agents list
void Session::addAgent(const Agent &agent) {
    Agent *ag = agent.clone();
    agents.push_back(ag);
}

void Session::setGraph(const Graph &graph) {
    g = graph;
}


void Session::setTreeType(const string& type) {
    if (type == "M")
        treeType = MaxRank;
    else if (type == "C")
        treeType = Cycle;
    else if (type == "R")
        treeType = Root;
}

// initialize from json file data of agents. caller responsibility to the delete pointer
Agent *Session::makeAgent(const string &type, int index) {
    if (type == "V") {
        g.infectNode(index);
        return new Virus(index);
    }
    else if (type == "C") {
        return new ContactTracer();
    }
    return nullptr; // if no matching type
}

void Session::nodeDisconnect(int node) {
    g.nodeDisconnect(node);
}

void Session::infectNode(const int nodeIndex) {
    addAgent(Virus(nodeIndex)); // add new agent to the agent's list
    g.infectNode(nodeIndex);
}

int Session::findHealthyNeighbor(const int nodeIndex) {
    return g.findHealthyNeighbor(nodeIndex);
}

void Session::fullyInfectNode(const int nodeIndex) {
    g.fullyInfect(nodeIndex);
}

// return a pointer to a tree on the heap. caller responsibility to delete.
Tree *Session::createBFS(int node) {
    Tree *root = Tree::createTree(*this,node);
    vector<bool> visited(g.graphSize(), false);
    vector<Tree*> queue = vector<Tree*>();
    queue.push_back(root);
    visited[node] = true;
    unsigned int nextBFSDequeue = 0;
    while (nextBFSDequeue < (queue.size())) {
        Tree *curr = queue[nextBFSDequeue];
        vector<int> neighbors = g.getNeighbors(curr->getRootLabel());
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                Tree *child = Tree::createTree(*this, neighbor);
                curr->addChild(child);
                visited[neighbor] = true;
                queue.push_back(child);
            }
        }
        nextBFSDequeue++;
    }
    return root;
}


