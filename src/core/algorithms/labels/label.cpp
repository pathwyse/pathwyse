#include "label.h"
#include "utils/logger.h"

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
    if(Parameters::getVerbosity() < 0) return;

    Logger::log("[ Label ]", VERB_STD);
    Logger::log("Node: ", node);
    if(predecessor)
        Logger::log("Predecessor", std::to_string(predecessor->getNode()));
    Logger::log("Direction", std::to_string(direction));
    Logger::log("Objective", std::to_string(objective));

    std::string snap;
    for(auto& s: snapshot)
        snap += std::to_string(s) + " ";
    Logger::log("Resource snapshot", snap);

    Logger::divider();
}

void Label::printPredecessors(){
    if(getPredecessor())
        getPredecessor()->printPredecessors();
    printLabel();
}
