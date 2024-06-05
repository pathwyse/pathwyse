#include "label_advanced.h"

/** Label management **/
//Constructors and destructors
LabelAdv::LabelAdv(const LabelAdv &obj) : Label(obj) {
    if(not obj.visited.empty()){
        visited = obj.visited;
        unreachable = obj.unreachable;
    }
}

//Init and updates
void LabelAdv::initVisited(int node, int n_nodes){
    this->visited = Bitset(n_nodes);
    visited.set(node);
    this->unreachable = this->visited;
}

void LabelAdv::updateLabel(int node, Label* predecessor) {
    Label::updateLabel(node, predecessor);

    if(not visited.empty()) {
        visited.set(node);
        unreachable.set(node);
    }
}

/** Operators **/
void LabelAdv::operator= (const LabelAdv& obj)  {
    if(this == &obj) return;

    Label::operator=(obj);
    if(not obj.visited.empty()){
        visited = obj.visited;
        unreachable = obj.unreachable;
    }
}

bool LabelAdv::operator== (LabelAdv& obj)  {
    if(visited != obj.visited or unreachable != obj.unreachable)
        return false;

    return Label::operator==(obj);
}

bool LabelAdv::operator!= (LabelAdv& obj)  {
    if(visited != obj.visited or unreachable != obj.unreachable)
        return true;

    return Label::operator!=(obj);
}
