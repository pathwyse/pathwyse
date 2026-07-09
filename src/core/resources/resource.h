#ifndef RESOURCE_H
#define RESOURCE_H

#include <iostream>
#include <limits>
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
        bucket_compatible = true;
        lower_bound_d = INFMINUSDOUBLE;
        upper_bound_d = INFPLUSDOUBLE;
    };

    virtual ~Resource() {delete data;}

    //General info
    void setName(std::string name) {this->name = name;}
    std::string getName(){return name;}
    bool isBucketCompatible(){return bucket_compatible;}

    //Starting value
    int getInitValue(){return init_value;}

    /** Bounds **/
    //Global bounds
    void setBounds(int lb, int ub){lower_bound = lb; upper_bound = ub;}
    void setBoundsDouble(double lb, double ub){lower_bound_d = lb; upper_bound_d = ub;}
    void setLB(int lower_bound){this->lower_bound = lower_bound;}
    void setUB(int upper_bound){this->upper_bound = upper_bound;}
    void setLBDouble(double v){lower_bound_d = v;}
    void setUBDouble(double v){upper_bound_d = v;}
    int getLB(){return lower_bound;}
    int getUB(){return upper_bound;}
    double getLBDouble(){return lower_bound_d;}
    double getUBDouble(){return upper_bound_d;}

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

    /** Double representation **/
    void setArcCostDouble(int i, int j, double cost) { data->setArcCostDouble(i, j, cost); }
    double getArcCostDouble(int i, int j) { return data->getArcCostDouble(i, j); }
    void setNodeCostDouble(int id, double cost) { data->setNodeCostDouble(id, cost); }
    double getNodeCostDouble(int id) { return data->getNodeCostDouble(id); }
    std::vector<double> getNodeCostsDouble() { return data->getNodeCostsDouble(); }
    double getMaxAbsValue() {
        double max = data->getMaxAbsValue();
        if (std::isfinite(lower_bound_d)) max = std::max(max, std::abs(lower_bound_d));
        if (std::isfinite(upper_bound_d)) max = std::max(max, std::abs(upper_bound_d));
        return max;
    }
    void applyScale(double scale) {
        data->applyScale(scale);
        if (std::isfinite(lower_bound_d)) lower_bound = (int)(lower_bound_d * scale);
        if (std::isfinite(upper_bound_d)) upper_bound = (int)(upper_bound_d * scale);
    }

    /** Resource data structure management **/
    void initData(bool compress_data = false, int n_nodes = 1) {compress_data ? data = new ResourceDataMap(n_nodes): data = new ResourceDataMatrix(n_nodes);}
    void setData(ResourceData* data) {this->data = data;}
    ResourceData* getData() {return data;}

    int getArcCost(int i, int j) {return data->getArcCost(i,j);}
    void setArcCost(int i, int j, int cost) {data->setArcCost(i,j, cost);}

    int getNodeCost(int i) {return data->getNodeCost(i);}
    std::vector<int> getNodeCosts() {return data->getNodeCosts();}
    void setNodeCost(int i, int cost) { data->setNodeCost(i, cost);}
    void setNodeCosts(std::vector<int> costs) { data->setNodeCosts(costs);}

    /** Resource behaviour definition **/

    virtual void init(int origin, int destination) {}
    virtual int extend(int current_value, int i, int j, bool direction = true) = 0;
    virtual int join(int current_value_forward, int current_value_backward, int i, int j) = 0;
    virtual int join(int current_value_forward, int current_value_backward, int node) = 0;
    virtual bool isFeasible(int current_value, int current_node = -1, double bounding = 1, bool direction = true) = 0;

protected:

    std::string name;

    int init_value;
    bool bucket_compatible;
    int lower_bound, upper_bound;
    double lower_bound_d, upper_bound_d;
    std::vector<int> node_lower_bound, node_upper_bound;

    ResourceData* data;
};

#endif
