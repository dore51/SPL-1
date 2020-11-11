#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
using namespace std;

class Graph{

public:
    Graph(vector<vector<int>> matrix);
    void infectNode(int nodeInd);
    bool isInfected(int nodeInd);
    void nodeDisconnect(unsigned int node);
    int findHealthyNeighbor(const int i);
    int graphSize() const;
    void fullyInfect(int nodeInd);
    bool isThereCarrierNotActed();
    vector<int> getNeighbors(unsigned int node);
    vector<vector<int>> getMatrix();
    vector<int> getInfectedNodes();

private:
    vector<int> carrierNodes; // nodes that got infected
    vector<int> fullyInfectedNodes; // infected nodes that acted at least once
    vector<vector<int>> edges;
};

#endif
