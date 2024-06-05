#ifndef RESOURCE_H
#define RESOURCE_H

#include <iostream>
#include "data/resource_data.h"
#include "utils/constants.h"

class Resource {

public:

    /** Resource management **/
    //Constructors and destructors
    Resource() {
        init_value = 0;
        lower_bound = INFMINUS;
        upper_bound = INFPLUS;
    };

    virtual ~Resource() {delete data;}

    //General info
    void setName(std::string name) {this->name = name;}
    std::string getName(){return name;}

    //Starting value
    int getInitValue(){return init_value;}
    void setInitValue(int init_value){this->init_value = init_value;}
    void increaseInitValue(int delta){init_value += delta;}
    void multiplyInitValue(float factor){init_value *= factor;}

    /** Bounds **/
    //Global bounds
    void setBounds(int lower_bound, int upper_bound){this->lower_bound = lower_bound; this->upper_bound = upper_bound;}
    void setLB(int lower_bound){this->lower_bound = lower_bound;}
    void setUB(int upper_bound){this->upper_bound = upper_bound;}
    void increaseUB(int delta) {upper_bound += delta;}
    void increaseLB(int delta) {lower_bound += delta;}
    void multiplyUB(float factor) {upper_bound *= factor;}
    void multiplyLB(float factor) {lower_bound *= factor;}
    int getLB(){return lower_bound;}
    int getUB(){return upper_bound;}

    //Node bounds
    void setNodeBounds(std::vector<int> node_lower_bound, std::vector<int> node_upper_bound) {
        this->node_lower_bound = node_lower_bound;
        this->node_upper_bound = node_upper_bound;
    }

    void setNodeBound(int n_nodes, int i, int node_lower_bound, int node_upper_bound){
        if(this->node_lower_bound.empty())
            this->node_lower_bound.resize(n_nodes, 0);
        if(this->node_upper_bound.empty())
            this->node_upper_bound.resize(n_nodes, 0);

        this->node_lower_bound[i] = node_lower_bound;
        this->node_upper_bound[i] = node_upper_bound;
    }

    void increaseNodeBound(int i, int delta) {
        if(not node_lower_bound.empty())
            node_lower_bound[i] += delta;
        if(not node_upper_bound.empty())
            node_upper_bound[i] += delta;
    }

    void multiplyNodeBound(int i, float factor) {
        if(not node_lower_bound.empty())
            node_lower_bound[i] *= factor;
        if(not node_upper_bound.empty())
            node_upper_bound[i] *= factor;
    }

    int getNodeLB(int node){return node_lower_bound[node];}
    int getNodeUB(int node){return node_upper_bound[node];}
    std::vector<int> & getNodesLB(){return node_lower_bound;}
    std::vector<int> & getNodesUB(){return node_upper_bound;}

    /** Scaling **/
    void scaleResource(float scaling){
        multiplyInitValue(scaling);
        multiplyUB(scaling);
        multiplyLB(scaling);

        for(int i = 0; i < node_lower_bound.size(); i++)
            multiplyNodeCost(i, scaling);

        data->scaleData(scaling);
    }
    
    /** Resource data structure management **/
    void initData(bool compress_data = false, int n_nodes = 1) {compress_data ? data = new ResourceDataMap(n_nodes): data = new ResourceDataMatrix(n_nodes);}
    void setData(ResourceData* data) {this->data = data;}
    ResourceData* getData() {return data;}

    int getArcCost(int i, int j) {return data->getArcCost(i,j);}
    void setArcCost(int i, int j, int cost) {data->setArcCost(i,j, cost);}
    void increaseArcCost(int i, int j, int delta) {data->increaseArcCost(i, j, delta);}
    void multiplyArcCost(int i, int j, float factor) {data->multiplyArcCost(i, j, factor);}

    int getNodeCost(int i) {return data->getNodeCost(i);}
    void setNodeCost(int i, int cost) { data->setNodeCost(i, cost);}
    void setNodeCosts(std::vector<int> costs) { data->setNodeCosts(costs);}
    void increaseNodeCost(int i, int delta){data->increaseNodeCost(i, delta);}
    void multiplyNodeCost(int i, float factor){data->multiplyNodeCost(i, factor);}

    /** Resource behaviour definition **/

    virtual void init(int origin, int destination) {}
    virtual int extend(int current_value, int i, int j, bool direction = true) = 0;
    virtual int join(int current_value_forward, int current_value_backward, int i, int j) = 0;
    virtual int join(int current_value_forward, int current_value_backward, int node) = 0;
    virtual bool isFeasible(int current_value, int current_node = -1, double bounding = 1, bool direction = true) = 0;

protected:

    std::string name;

    int init_value;

    int lower_bound, upper_bound;
    std::vector<int> node_lower_bound, node_upper_bound;

    ResourceData* data;
};

#endif
