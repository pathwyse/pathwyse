#include "label.h"

/** Label management **/
//Constructors and destructors
Label::Label(const Label &obj)  {
    node = obj.node;
    direction = obj.direction;
    predecessor = obj.predecessor;
    objective = obj.objective;
    snapshot = obj.snapshot;
}

//Init and updates
void Label::initLabel(int node, Label* predecessor, bool direction, int n_res){
    this->node = node;
    this->predecessor = predecessor;
    this->direction = direction;
    this->snapshot.resize(n_res, 0);
}

void Label::updateLabel(int node, Label* predecessor){
    this->node = node;
    this->predecessor = predecessor;
}

/** Operators **/
void Label::operator= (const Label& obj)  {
    if(this == &obj) return;

    node = obj.node;
    direction = obj.direction;
    predecessor = obj.predecessor;
    objective = obj.objective;
    snapshot = obj.snapshot;
}

bool Label::operator== (const Label& obj)  {
    if(node != obj.node or direction != obj.direction or objective != obj.objective or snapshot != obj.snapshot)
        return false;

    return true;
}

bool Label::operator!= (const Label& obj) {
    if (node != obj.node or direction != obj.direction or objective != obj.objective or snapshot != obj.snapshot)
        return true;

    return false;
}

/** Output management **/
void Label::printLabel() {
    if(Parameters::getVerbosity() < 0)
        return;

    std::cout<<"------------"<<std::endl;
    std::cout<<"Node: "<< node << std::endl;
    if(predecessor)
        std::cout<<"Predecessor: "<< predecessor->getNode() << std::endl;
    std::cout<<"Direction: " << direction << std::endl;
    std::cout<<"Objective: " << objective << std::endl;
    std::cout<<"Resource snapshot: " << std::endl;
    for(auto& s: snapshot)
        std::cout << s << " "  << std::endl;
    std::cout<<"------------"<<std::endl;
}

void Label::printPredecessors(){
    if(getPredecessor())
        getPredecessor()->printPredecessors();
    printLabel();
}
