#ifndef LABEL_ADVANCED_H
#define LABEL_ADVANCED_H

#include "label.h"
#include "utils/bitset.h"

class LabelAdv: public Label {

public:

    /** Label management **/
    //Constructors and destructors
    LabelAdv() = default;
    LabelAdv(const LabelAdv &obj);

    ~LabelAdv() override = default;

    //Init and updates
    void initVisited(int node, int n_nodes);
    void updateLabel(int node, Label* predecessor);

    //Visited and unreachable management
    bool isReachable(int node) {return unreachable.get(node) == 0;}
    void updateUnreachables(Bitset & mask) {unreachable &= mask; visited &= mask;}
    Bitset & getVisited(){return visited;}
    Bitset & getUnreachable(){return unreachable;}
    void setUnreachable(int node){unreachable.set(node);}

    //Extension target and label type management
    int getExtensionTarget(){return extension_target;}
    void setExtensionTarget(int extension_target){this->extension_target = extension_target;}
    short getLabelType(){return label_type;}
    void setLabelType(short label_type){this->label_type = label_type;}

    /** Operators **/
    void operator= (const LabelAdv& obj);
    bool operator== (LabelAdv& obj);
    bool operator!= (LabelAdv& obj);

    /** Output management **/
    void printVisited() {std::cout<<visited<<std::endl;}
    void printUnreachable() {std::cout<<unreachable<<std::endl;}

private:
    Bitset visited;
    Bitset unreachable;
    short label_type = UNDEFINED;
    int extension_target = ALL;
};


#endif