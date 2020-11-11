#include "../include/Graph.h"
#include <vector>
using namespace std;

Graph::Graph(vector<vector<int>> matrix): carrierNodes(vector<int>()), fullyInfectedNodes(vector<int>()), edges(matrix) {
    for (unsigned int i = 0;i<matrix.size();i++) {
        carrierNodes.push_back(0);
        fullyInfectedNodes.push_back(0);
    }
}

// node got infected for first time
void Graph::infectNode(int nodeInd) {
    if (nodeInd >= 0)
        carrierNodes[nodeInd] = 1;
}

// returns true if the node got infected
bool Graph::isInfected(int nodeInd){
    if (nodeInd >= 0)
        return carrierNodes[nodeInd] == 1;
    return false;
}

// disconnect all edges connected to node
void Graph::nodeDisconnect(unsigned int node) {
    if (node >= 0 && node < edges.size()){
        for (unsigned int i = 0; i < edges[node].size(); i++) {
            edges[node][i] = 0;
        }
        for (unsigned int i = 0; i < edges.size(); i++) {
            edges[i][node] = 0;
        }
    }
}

// return's healthy neighbor with smallest node index or -1 if none
int Graph::findHealthyNeighbor(const int index) {
    for (unsigned int neighbor = 0; neighbor < edges[index].size(); neighbor++ ){
        int isNeighbor = edges[index][neighbor];
        if (isNeighbor == 1 && !isInfected(neighbor)){
            return neighbor;
        }
    }
    return -1; // no healthy neighbor

}

// returns number of nodes in the graph
int Graph::graphSize() const {
    return edges.size();
}

// returns an index's vector of node neighbors
vector<int> Graph::getNeighbors(unsigned int node) {
    vector<int> neighbors;
    if (node >= 0 && node < edges.size()) {
        for (unsigned int index = 0; index < edges[node].size(); index++) {
            if ((edges[node][index]) == 1)
                neighbors.push_back(index);
        }
    }
    return neighbors;
}

// returns a copy of the matrix
vector<vector<int>> Graph::getMatrix() {
    return edges;
}

// returns all the nodes that got infected
vector<int> Graph::getInfectedNodes() {
    vector<int> outputInfected;
    for (unsigned int index = 0; index < fullyInfectedNodes.size(); index++) {
        if (fullyInfectedNodes[index] == 1)
            outputInfected.push_back(index);
    }
    return outputInfected;
}

// now now is an active virus
void Graph::fullyInfect(int nodeInd) {
    if (nodeInd >= 0)
        fullyInfectedNodes[nodeInd] = 1;
}

// returns true if there's an infected node that has not been active yet.
bool Graph::isThereCarrierNotActed() {
    for (unsigned int i = 0; i < carrierNodes.size(); i++){
        if (carrierNodes[i] == 1 && fullyInfectedNodes[i] == 0) // he's a carrier
            return true;
    }
    return false;
}
