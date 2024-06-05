#include "problem.h"
#include <iostream>
#include <iterator>
#include <sstream>

/** Problem Management **/
//Constructors and destructors
Problem::Problem() {
    name = "undefined";
    n_nodes = 0;
    origin = 0;
    destination = 0;
    n_res = 0;
    cycles = false;
    complete = false;
    objective = nullptr;
    bound_labels = nullptr;
    setStatus(PROBLEM_INDETERMINATE);
}

Problem::~Problem() {
    delete objective;
    delete bound_labels;
    for(auto& r: resources)
        delete r;
}

//Initialize Problem
void Problem::initProblem(){
    //Use Data Compression for sparse data
    compress_data = not complete and n_nodes > Parameters::getCompressionThreshold();

    //Duplicate origin node
    if(origin == destination)
        destination = n_nodes++;

    //Initialize Network
    network.initGraph(compress_data, n_nodes, complete);

    //Initialize Objective data structures
    initObjective();

    //Initialize Data Collection
    initDataCollection();
}

void Problem::printStatus(){
    std::string status;

    switch(problem_status){
        case PROBLEM_INDETERMINATE:
            status = "Indeterminate";
            break;
        case PROBLEM_INFEASIBLE:
            status = "Infeasible";
            break;
        case PROBLEM_FEASIBLE:
            status = "Feasible";
            break;
        default:
            break;
    }

    std::cout << "Problem Status: " << status <<  std::endl;
}

/** Objective and Resource management **/
//Initialize Objective data structures
void Problem::initObjective(Resource *objective) {
    //If a custom objective is not defined, use default objective
    if(not objective)
        objective = new DefaultCost();
    this->objective = objective;

    //Initialize data structures
    this->objective->initData(compress_data, n_nodes);
}

//Initialize a resource Data Object
int Problem::addResource(int type) {
    Resource* res;
    int index;

    switch(type){
        case RES_CAPACITY:
            res = new Capacity();
        break;
        case RES_TIME:
            res = new Time();
        break;
        case RES_TIMEWINDOWS:
            res = new TimeWindow();
        break;
        case RES_NODELIM:
            res = new NodeLim();
        break;
        default:
            std::cout << "Inserted type does not correspond to any default resource" << std::endl;
        break;
    }

    resources.push_back(res);
    res->initData(compress_data, n_nodes);

    return index;
}

void Problem::createResources(std::vector<int> & resources_type){
    for(auto rt: resources_type)
        addResource(rt);
}

void Problem::setArcConsumption(int i, int j,  std::vector<int> consumption) {
    for(int r = 0; r < resources.size(); r++)
        resources[r]->setArcCost(i, j, consumption[r]);
}

void Problem::setNodeConsumption(int id, std::vector<int> consumption) {
    for(int r = 0; r < resources.size(); r++)
        if(consumption[r] >= 0)
            resources[r]->setNodeCost(id, consumption[r]);
}

void Problem::scaleObjective(float scaling) {
    objective->scaleResource(scaling);
}

void Problem::scaleResource(int id, float scaling) {
    resources[id]->scaleResource(scaling);
}

void Problem::scaleAllData(float scaling) {
    objective->multiplyInitValue(scaling);
    objective->multiplyUB(scaling);
    objective->multiplyLB(scaling);

    for(auto r: resources){
        r->multiplyInitValue(scaling);
        r->multiplyUB(scaling);
        r->multiplyLB(scaling);
    }

    for(int i = 0; i < n_nodes; i++) {
        objective->multiplyNodeCost(i, scaling);
        objective->multiplyNodeBound(i, scaling);
        for(auto r: resources){
            r->multiplyNodeCost(i, scaling);
            r->multiplyNodeBound(i, scaling);
        }

        auto& neighbors = network.getNeighbors(i, true);
        for(auto j: neighbors) {
            objective->multiplyArcCost(i, j, scaling);
            for(auto r: resources)
                r->multiplyArcCost(i, j, scaling);
        }
    }
}

/** Read from instance file **/
//Read and initialize data (compact reader)
void Problem::readSparseProblem(std::string file_name) {
    std::ifstream f;
    std::string line, key, label, separator, res_name, res_type, data_type;
    std::vector<std::string> tokens;
    bool symmetric = false, use_depot = false;
    int res_id, i, j, di, dj, xval, yval;
    int lb, ub, cost, consumption;
    directed = true;

    if(Parameters::getVerbosity() >= 3)
        std::cout<<"Reading problem data: "<<file_name<<std::endl;

    //If present, override origin and destination with console input
    if(Parameters::getOrigin() != -1)
        this->origin = Parameters::getOrigin();
    if(Parameters::getDestination() != -1)
        this->destination = Parameters::getDestination();

    //Read from file
    f.open(file_name.c_str());

    while(readNextLine(f, line, tokens, key)) {
        //Read initialization data
        if (key == "NAME") this->name = tokens[2];
        else if (key == "SIZE") this->n_nodes = std::stoi(tokens[2]);
        else if (key == "DIRECTED") directed = std::stoi(tokens[2]);
        else if (key == "CYCLIC") this->cycles = std::stoi(tokens[2]);
        else if (key == "SYMMETRIC") symmetric = std::stoi(tokens[2]);
        else if (key == "COMPLETE") {
            this->complete = std::stoi(tokens[2]);
            collector.collect("complete", complete);
        }
        else if (key == "RESOURCES") this->n_res = std::stoi(tokens[2]);
        else if (key == "ORIGIN") {
            if (Parameters::getOrigin() == -1)
                this->origin = std::stoi(tokens[2]);
        }
        else if (key == "DESTINATION") {
            if (Parameters::getDestination() == -1)
                this->destination = std::stoi(tokens[2]);
        }
        else if (key == "RES_TYPE") {
            //Duplicate origin node if necessary
            if (origin == destination)
                use_depot = true;

            //Initialize problem
            initProblem();

            readNextLine(f, line, tokens, key);

            while (key != "END") {
                //Initialize resource objects
                res_type = tokens[1];
                if (res_type == "CAP") addResource(RES_CAPACITY);
                else if (res_type == "TIME") addResource(RES_TIME);
                else if (res_type == "TW") addResource(RES_TIMEWINDOWS);
                else if (res_type == "NODELIM") addResource(RES_NODELIM);
                readNextLine(f, line, tokens, key);
            }
        }
        else if (key == "RES_BOUND") {
            readNextLine(f, line, tokens, key);

            //Read lower bound and upperbound for each resource
            while (key != "END") {
                res_id = tokens[0].at(1) - '0';
                lb = std::stoi(tokens[1]);
                ub = std::stoi(tokens[2]);
                if (res_id == 0 and Parameters::getCriticalUB() != -1)
                    ub = Parameters::getCriticalUB();
                resources[res_id]->setBounds(lb, ub);
                readNextLine(f, line, tokens, key);
            }
        }
        else if (key == "NODE_DATA") {
            //Read node data: for each node i, read coordinates (x, y), cost and resource consumption
            readNextLine(f, line, tokens, key);
            auto header = tokens;
            if(std::find(header.begin(), header.end(), "x") != header.end())
                network.initCoord();
            readNextLine(f, line, tokens, key);
            while (key != "END") {
                i = std::stoi(tokens[0]);
                for (int index = 1; index < header.size(); index++) {
                    data_type = header[index];
                    if (data_type == "x") {
                        xval = std::stoi(tokens[index++]);
                        yval = std::stoi(tokens[index]);
                        network.setxy(i, xval, yval);
                    }
                    else if (data_type == "COST") {
                        cost = std::stoi(tokens[index]);
                        objective->setNodeCost(i, cost);
                    }
                    else {
                        res_id = data_type.at(1) - '0';
                        consumption = std::stoi(tokens[index]);
                        resources[res_id]->setNodeCost(i, consumption);
                    }
                }
                readNextLine(f, line, tokens, key);
            }
        }
        else if (key == "ARC_DATA") {
            //Read node data: for each arc i-j, read coordinates (x, y), costs and resource consumption
            //Add arcs j-i if the graph is not directed
            //If the origin node is duplicated, add arcs for the destination

            readNextLine(f, line, tokens, key);
            auto header = tokens;
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                i = std::stoi(tokens[0]);
                j = std::stoi(tokens[1]);

                //Add an arc to the network
                network.setArc(i, j);
                n_arcs++;

                if (not directed) {
                    network.setArc(j, i);
                    n_arcs++;
                }

                if (use_depot and (i == origin or j == origin)) {
                    di = i == origin ? destination : i;
                    dj = j == origin ? destination : j;

                    network.setArc(di, dj);
                    n_arcs++;

                    if (not directed) {
                        network.setArc(dj, di);
                        n_arcs++;
                    }
                }

                for (int index = 2; index < header.size(); index++) {
                    data_type = header[index];

                    if (data_type == "COST") {
                        cost = std::stoi(tokens[index]);

                        objective->setArcCost(i, j, cost);
                        if (not directed)
                            objective->setArcCost(j, i, cost);

                        if (use_depot and (i == origin or j == origin)) {
                            objective->setArcCost(di, dj, cost);
                            if (not directed)
                                objective->setArcCost(dj, di, cost);
                        }
                    }
                    else {
                        res_id = data_type.at(1) - '0';
                        consumption = std::stoi(tokens[index]);
                        resources[res_id]->setArcCost(i, j, consumption);

                        if (not directed)
                            resources[res_id]->setArcCost(j, i, consumption);

                        if (use_depot and (i == origin or j == origin)) {
                            resources[res_id]->setArcCost(di, dj, consumption);

                            if (not directed)
                                resources[res_id]->setArcCost(dj, di, consumption);
                        }
                    }
                }
                readNextLine(f, line, tokens, key);
            }
        }
        else if (key == "RES_NODE_BOUND") {
            //For each node i, add lower and upper bound for a given resource
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                res_id = tokens[0].at(1) - '0';
                i = std::stoi(tokens[1]);
                lb = std::stoi(tokens[2]);
                ub = std::stoi(tokens[3]);
                resources[res_id]->setNodeBound(n_nodes, i, lb, ub);
                readNextLine(f, line, tokens, key);
            }
        }
    }
    f.close();

    if(Parameters::getVerbosity() >= 3)
        std::cout<<"Reading problem data complete"<<std::endl;

    //Initialize resources
    for(auto r: resources)
        r->init(origin, destination);

    printProblem();
    collectData();
}

//Read and initialize data (extended reader)
void Problem::readProblem(std::string file_name) {
    std::ifstream f;
    std::string line, key, label, separator, res_name, res_type;
    std::vector<std::string> tokens;
    int res_id, i, j, cluster_id;
    bool symmetric = false, use_depot = false;
    int lb, ub, cost, consumption;

    directed = true;

    if(Parameters::getVerbosity() >= 0)
        std::cout<<"Reading problem data: "<<file_name<<std::endl;

    //If present, override origin and destination with console input
    if(Parameters::getOrigin() != -1)
        this->origin = Parameters::getOrigin();

    if(Parameters::getDestination() != -1)
        this->destination = Parameters::getDestination();

    //Read from file
    f.open(file_name.c_str());

    while(readNextLine(f, line, tokens, key)) {
        //Read initialization data
        if (key == "NAME") this->name = tokens[2];
        else if (key == "SIZE") this->n_nodes = std::stoi(tokens[2]);
        //else if (key == "CLUSTERS") this->n_clusters = std::stoi(tokens[2]);
        else if (key == "DIRECTED") directed = std::stoi(tokens[2]);
        else if (key == "CYCLIC") this->cycles = std::stoi(tokens[2]);
        else if (key == "SYMMETRIC") symmetric = std::stoi(tokens[2]);
        else if (key == "COMPLETE") {
            this->complete = std::stoi(tokens[2]);
            collector.collect("complete", complete);
        }
        else if (key == "RESOURCES") this->n_res = std::stoi(tokens[2]);
        else if (key == "ORIGIN") {
            if(Parameters::getOrigin() == -1)
                this->origin = std::stoi(tokens[2]);
        }
        else if (key == "DESTINATION") {
            if(Parameters::getDestination() == -1)
                this->destination = std::stoi(tokens[2]);
        }
        else if (key == "RES_TYPE") {
            //Duplicate origin node if necessary
            if (origin == destination)
                use_depot = true;

            //Initialize problem
            initProblem();

            readNextLine(f, line, tokens, key);

            while(key != "END") {
                //Initialize resource objects
                res_type = tokens[1];
                if (res_type == "CAP") addResource(RES_CAPACITY);
                else if (res_type == "TIME") addResource(RES_TIME);
                else if (res_type == "TW") addResource(RES_TIMEWINDOWS);
                else if (res_type == "NODELIM") addResource(RES_NODELIM);
                readNextLine(f, line, tokens, key);
            }
        }

        else if (key == "RES_BOUND") {
            //Read lower bound and upperbound for each resource
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                res_id = std::stoi(tokens[0]);
                lb = std::stoi(tokens[1]);
                ub = std::stoi(tokens[2]);
                if(res_id == 0 and Parameters::getCriticalUB() != -1)
                    ub = Parameters::getCriticalUB();
                resources[res_id]->setBounds(lb, ub);
                readNextLine(f, line, tokens, key);
            }
        }


        else if (key == "RES_NODE_BOUND") {
            //For each node i, add lower and upper bound for a given resource
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                res_id = std::stoi(tokens[0]);
                i = std::stoi(tokens[1]);
                lb = std::stoi(tokens[2]);
                ub = std::stoi(tokens[3]);
                resources[res_id]->setNodeBound(n_nodes, i, lb, ub);
                readNextLine(f, line, tokens, key);
            }
        }

        else if (key == "EDGE_COST") {
            //For each arc i-j, read a cost
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                i = std::stoi(tokens[0]);
                j = std::stoi(tokens[1]);
                cost = std::stoi(tokens[2]);
                objective->setArcCost(i, j, cost);

                //Add an arc to the network
                network.setArc(i, j);
                n_arcs++;

                if (!directed) {
                    objective->setArcCost(j, i, cost);
                    network.setArc(j, i);
                    n_arcs++;
                }

                if (use_depot and (i == origin or j == origin)) {
                    i == origin ? i = destination : j = destination;
                    objective->setArcCost(i, j, cost);
                    network.setArc(i, j);
                    n_arcs++;

                    if (!directed) {
                        objective->setArcCost(j, i, cost);
                        network.setArc(j, i);
                        n_arcs++;
                    }
                }

                readNextLine(f, line, tokens, key);
            }
        }

        else if (key == "NODE_COST") {
            //For each node i, read a cost
            readNextLine(f, line, tokens, key);
            while (key != "END") {
                i = std::stoi(tokens[0]);
                cost = std::stoi(tokens[1]);
                objective->setNodeCost(i, cost);

                if (use_depot and i == origin)
                    objective->setNodeCost(destination, 0);

                readNextLine(f, line, tokens, key);
            }
        }

        else if(key == "EDGE_CONSUMPTION") {
            //For each arc i-j, read a resource consumption
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                res_id = std::stoi(tokens[0]);
                i = std::stoi(tokens[1]);
                j = std::stoi(tokens[2]);
                consumption = std::stoi(tokens[3]);
                resources[res_id]->setArcCost(i, j, consumption);

                if (!directed)
                    resources[res_id]->setArcCost(j, i, consumption);

                if (use_depot and (i == origin or j == origin)) {
                    i == origin ? i = destination : j = destination;
                    resources[res_id]->setArcCost(i, j, consumption);

                    if (!directed)
                        resources[res_id]->setArcCost(j, i, consumption);
                }
                readNextLine(f, line, tokens, key);
            }
        }

        else if (key == "NODE_CONSUMPTION") {
            //For each node i, read a resource consumption
            readNextLine(f, line, tokens, key);

            while (key != "END") {
                res_id = std::stoi(tokens[0]);
                i = std::stoi(tokens[1]);
                consumption = std::stoi(tokens[2]);
                resources[res_id]->setNodeCost(i, consumption);

                if (use_depot and i == origin)
                    resources[res_id]->setNodeCost(destination, 0);

                readNextLine(f, line, tokens, key);
            }
        }

        else if (key == "COORD") {
            //For each node i, read coordinates (x,y)
            readNextLine(f, line, tokens, key);
            network.initCoord();
            int xval, yval;

            while (key != "END") {
                i = std::stoi(tokens[0]);
                xval = std::stoi(tokens[1]);
                yval = std::stoi(tokens[2]);
                network.setxy(i, xval, yval);
                readNextLine(f, line, tokens, key);
            }
        }

    }

    f.close();

    if(Parameters::getVerbosity() >= 0){
        std::cout<<"Reading problem data complete"<<std::endl;
        std::cout<<"--------------------"<<std::endl;
    }

    //Initialize resources
    for(auto r: resources)
        r->init(origin, destination);

    printProblem();
    collectData();
}

//Support methods for read procedure
void Problem::getTokens(std::string & line, std::vector<std::string> & tokens){
    tokens.clear();
    std::istringstream iss(line);
    tokens = {std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>{}};
}

bool Problem::readNextLine(std::ifstream & f, std::string & line, std::vector<std::string> & tokens, std::string & key){
    if(getline(f, line)) {
        getTokens(line, tokens);
        key = tokens[0];
        return true;
    }
    return false;
}

/** Output management **/
//Prints problem Data
void Problem::printProblem(){
    if(Parameters::getVerbosity() < 1)
        return;

    std::cout<< "Problem: " << name << std::endl;
    std::cout<< "Number of resources: " << n_res << std::endl;
    std::cout<< "Number of nodes: " << getNumNodes() << std::endl;
    std::cout<<"Origin: " << this->origin << std::endl;
    std::cout<<"Destination: " << this->destination << std::endl;
    std::cout<<"Upperbounds: ";
    for(auto & r: resources)
        std::cout << r->getUB()<< " ";
    std::cout<<std::endl;
    std::cout << "--------------------" << std::endl;
}

/** Data collection **/
void Problem::initDataCollection(){
    collector = DataCollector("Problem");

    if(not Parameters::isCollecting())
        return;

    collector.init("problem_name", "");
    collector.init("nodes", 0);
    collector.init("arcs", 0);
    collector.init("arcs_per_node", 0.0);
    collector.init("complete", -1);
    collector.init("symmetric", -1);
    collector.init("directed", -1);
    collector.init("cyclic", -1);
    collector.init("nres", 0);
    collector.init("nres_complex", 0);
    collector.init("origin", -1);
    collector.init("destination", -1);
    collector.init("obj_init", 0);
    collector.init("obj_lb", 0);
    collector.init("obj_ub", 0);
}

void Problem::collectData(){
    if(not Parameters::isCollecting())
        return;
    collector.collect("problem_name", name);
    collector.collect("nodes", n_nodes);
    collector.collect("arcs", n_arcs);
    collector.collect("arcs_per_node", (double)n_arcs/n_nodes);
    collector.collect("directed", directed);
    collector.collect("cyclic", cycles);
    collector.collect("nres", n_res);
    collector.collect("origin", origin);
    collector.collect("destination", destination);
    collector.collect("obj_init", getObj()->getInitValue());
    collector.collect("obj_lb", getObj()->getLB());
    collector.collect("obj_ub", getObj()->getUB());
    for(int i = 0; i < resources.size(); i++) {
        auto res = getRes(i);
        std::string tag = "r" + std::to_string(i) + "_";
        collector.init(tag+"init", res->getInitValue());
        collector.init(tag+"lb", res->getLB());
        collector.init(tag+"ub", res->getUB());
    }

    collector.setHeader();
    collector.saveRecord();
    collector.writeData();
}
