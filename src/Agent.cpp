#include "../include/Agent.h"
#include "../include/Session.h"
using namespace std;

//// CLASS AGENT ////

// constructor
Agent::Agent(): isActed(false) {}

// virtual destructor because of abstract class
Agent::~Agent() = default;

Agent *Virus::clone() const {
    return new Virus(*this);
}


//// SUB-CLASS CONTACT TRACER  ////

// default constructor
ContactTracer::ContactTracer() = default;

Agent *ContactTracer::clone() const {
    return new ContactTracer(*this);
}

void ContactTracer::act(Session& session) {
    int node = session.dequeueInfected();
    if (node >= 0) { // queue is not empty
        Tree *bfsTree = session.createBFS(node); // on heap - our responsibility to delete
        int toDisconnect = bfsTree->traceTree();
        session.nodeDisconnect(toDisconnect);
        delete bfsTree;
    }
}


//// SUB-CLASS VIRUS  ////

// constructor
Virus::Virus(int nodeInd): nodeInd(nodeInd){}

void Virus::act(Session& session) {
    if (!isActed) { // first time acting
        session.enqueueInfected(nodeInd);
        isActed = true;
        session.fullyInfectNode(nodeInd);
    }

    int nodeToInfect = session.findHealthyNeighbor(nodeInd);
    if (nodeToInfect >= 0) { // assuming nodes indexes are natural numbers
        session.infectNode(nodeToInfect);
    }
}

