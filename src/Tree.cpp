#include "../include/Tree.h"
#include "../include/Session.h"

class Session;
using namespace std;

//// CLASS TREE ////

// constructor
Tree::Tree(int rootLabel): node(rootLabel), children(vector<Tree*>()), timeDiscovered(0) {};

// copy constructor
Tree::Tree(const Tree &other): node(other.node), children(vector<Tree*>()), timeDiscovered(0) {
    copy(other);
}

// destructor
Tree::~Tree() {
    clear();
}

// Assignment operator
Tree & Tree::operator=(const Tree &other) {
    if (this != &other) {
        Tree *temp = other.clone();
        clear();
        steal(*temp);
        delete temp;
    }
    return *this;
}

// Move Constructor
Tree::Tree(Tree &&other): node(other.node), children(vector<Tree*>()), timeDiscovered(other.timeDiscovered) {
    steal(other);
}

// Move Assignment
Tree& Tree::operator=(Tree &&other) {
    if (this != &other) {
        clear();
        steal(other);
    }
    return *this;
}

// create another copy of other
void Tree::copy(const Tree &other) {
    node = other.node;
    timeDiscovered = other.timeDiscovered;
    for (unsigned int i = 0; i < other.children.size(); i++){
        Tree *child = other.children[i]->clone();
        children.push_back(child);
    }
}

// steal all other's resources
void Tree::steal(Tree &other) {
    node = other.node;
    timeDiscovered = other.timeDiscovered;
    children = vector<Tree*>();
    for (unsigned int index = 0;index<other.children.size();index++) {
        Tree *newTree = (other.children[index]);
        children.push_back(newTree);
        other.children[index] = nullptr;
    }
    other.children.clear();
}

void Tree::clear() {
    for (unsigned int i = 0;i<children.size();i++) {
        if (children[i]) {
            delete children[i];
            children[i] = nullptr;
        }
    }
    children.clear();
}
// pass by reference - safe but not efficient
void Tree::addChild(const Tree& child){
    Tree *tree = child.clone();
    children.push_back(tree);
}

// pass by pointer - less safe but efficient
void Tree::addChild(Tree* child){
    children.push_back(child);
}


Tree *Tree::createTree(const Session &session, int rootLabel) {
    TreeType type = session.getTreeType();
    if (type == MaxRank){
        return new MaxRankTree(rootLabel);
    }
    else if (type == Cycle){
        return new CycleTree(rootLabel,session.getCycle());
    }
    else if (type == Root){
        return new RootTree(rootLabel);
    }
    return nullptr; // no matching tree type
}


int Tree::getRootLabel() const {
    return node;
}

bool Tree::isLeaf() const  {
    return (children.empty());
}

int Tree::getNumOfChildren() const{
    return children.size();
}

vector<Tree *> Tree::getChildren() {
    return children;
}

// add to the input vector object from type Point4D, which each point keep a node data: rootLabel, number of children, depth, and time discovered in the BFS algorithm.
void Tree::getTreeData(vector<Point4D> & vec, Tree* root, int depth)  {
    Point4D point(root->getRootLabel(), root->getNumOfChildren(), depth, timeDiscovered);
    vec.push_back(point);
    vector<Tree*> curr_children = getChildren();
    for (unsigned int j = 0 ; j < curr_children.size(); j++){
        curr_children[j]->getTreeData(vec, curr_children[j], depth + 1);
    }
}

//// CLASS CYCLE TREE ////

// CycleTree constructor
CycleTree::CycleTree(int rootLabel, int currCycle): Tree(rootLabel), currCycle(currCycle) { }


Tree* CycleTree::clone() const {
    return new CycleTree(*this);
}

// iterate over the left-most branch on the tree (bound-counter) times or until leftmost child and return's the node index
int Tree::Trace(int counter, int currCycle){
    if (counter < currCycle && !isLeaf()){
        return children[0]->Trace(counter+1, currCycle);
    }
    return node;
}

int CycleTree::traceTree() {
    return Trace(0, currCycle);
}

//// CLASS MaxRank TREE ////

// MaxRankTree constructor
MaxRankTree::MaxRankTree(int rootLabel) : Tree(rootLabel) {}

Tree* MaxRankTree::clone() const {
    return new MaxRankTree(*this);
}

int MaxRankTree::traceTree() {
    vector<Point4D> nodes;
    getTreeData(nodes, this, 0);
    return findMaxDegree(nodes);
}

// finding the node with the maximum rank
int MaxRankTree::findMaxDegree(std::vector<Point4D> & nodes) {
    Point4D winner(-1, -1, -1,-1);
    for (Point4D point: nodes){
        int currDeg = point.getDegree();
        if (currDeg > winner.getDegree()) { // compare the degrees of the nodes
            winner = point;
        }
        else if (point.getDegree() == winner.getDegree()) {
            int currDepth = point.getDepth();
            int winnerDepth = winner.getDepth();
            if (currDepth < winnerDepth) // compare the depth of the nodes
                winner = point;
            else if (currDepth == winnerDepth)
                if (point.getTimeDiscovered() < winner.getTimeDiscovered()) // compare the node's time of discover
                    winner = point;
        }
    }
    return winner.getIndex();
}

//// CLASS Root TREE ////

// RootTree constructor
RootTree::RootTree(int rootLabel) : Tree(rootLabel) { }

int RootTree::traceTree() {
    return getRootLabel();
}

Tree *RootTree::clone() const {
    return new RootTree(*this);
}


//// CLASS Point4D ////

Point4D::Point4D(int index, int degree, int depth,int timeDiscovered): index(index), degree(degree), depth(depth), timeDiscovered(timeDiscovered)  {}

int Point4D::getIndex() {
    return index;
}

int Point4D::getDegree() {
    return degree;
}

int Point4D::getDepth() {
    return depth;
};

int Point4D::getTimeDiscovered() {
    return timeDiscovered;
};
