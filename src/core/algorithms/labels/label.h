#ifndef LABEL_H
#define LABEL_H
#include <iostream>
#include "utils/constants.h"
#include "utils/param.h"

class Label{

public:

    /** Label management **/
    //Constructors and destructors
    Label() {objective = UNKNOWN;}
    Label(const Label &obj);

    virtual ~Label() = default;

    //Init and updates
    void initLabel(int node, Label* predecessor, bool direction, int n_res);
    void updateLabel(int node, Label* predecessor);

    //Resources management
    int getObjective(){return objective;}
    void setObjective(int objective){this->objective = objective;}

    int getSnapshot(int resID) {return snapshot[resID];}
    std::vector<int> & getSnapshot() {return snapshot;}
    void setSnapshot(int resID, int value){snapshot[resID] = value;}
    void setSnapshot(std::vector<int> snapshot) {this->snapshot = snapshot;}

    /** Queries **/
    int getNode(){return node;}
    bool getDirection(){return direction;}
    Label* getPredecessor(){return predecessor;}
    bool isValid(){return objective != UNKNOWN;}

    /** Operators **/
    void operator= (const Label& obj);
    bool operator== (const Label& obj);
    bool operator!= (const Label& obj);

    /** Output management **/
    void printLabel();
    void printPredecessors();

protected:

    int node;
    bool direction;
    Label* predecessor;

    //Resources
    int objective;
    std::vector<int> snapshot;
};

#endif