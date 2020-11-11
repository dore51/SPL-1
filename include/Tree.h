#ifndef TREE_H_
#define TREE_H_

#include <vector>
class Session;
class Point4D;

class Tree{
public:
    // rule of 5
    Tree(int rootLabel);
    Tree(const Tree& other);
    Tree (Tree &&other);
    Tree& operator=(const Tree &other);
    Tree& operator=(Tree &&other);
    virtual ~Tree();
    // functions
    void addChild(const Tree& child);
    void addChild(Tree *child);
    int getRootLabel() const;
    bool isLeaf() const;
    int getNumOfChildren() const;
    virtual int traceTree()=0;
    virtual Tree* clone() const =0 ;
    static Tree* createTree(const Session& session, int rootLabel);

protected:
    // fields
    int node;
    std::vector<Tree*> children;
    int timeDiscovered;
    // functions
    void getTreeData(std::vector<Point4D> &vec, Tree *root, int depth);
    int Trace(int counter, int bound);
    void copy(const Tree& other);
    void clear();
    void steal(Tree &other);
    std::vector<Tree*> getChildren();
};


class CycleTree: public Tree{
public:
    CycleTree(int rootLabel, int currCycle);
    virtual Tree* clone() const;
    virtual int traceTree();

protected:

private:
    // fields
    int currCycle;

};


class MaxRankTree: public Tree{
public:
    MaxRankTree(int rootLabel);
    virtual Tree* clone() const;
    virtual int traceTree();

private:
    int findMaxDegree(std::vector<Point4D> & vector);
};


class RootTree: public Tree{
public:
    RootTree(int rootLabel);
    virtual Tree* clone() const;
    virtual int traceTree();
};


class Point4D{
private:
    int index;
    int degree;
    int depth;
    int timeDiscovered;

public:
    Point4D (int index,int degree, int depth,int timeDiscovered);
    int getIndex();
    int getDegree ();
    int getDepth();
    int getTimeDiscovered();
};

#endif
