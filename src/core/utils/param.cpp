#include <iostream>
#include <sstream>
#include <math.h>
#include <filesystem>
#include "param.h"
/**Default Values**/

std::string Parameters::param_path = "pathwyse.set";

/**Solver Parameters**/
std::string Parameters::instance_path = "input.txt";
int Parameters::verbosity = 2;

//Algorithm selection parameters
std::string Parameters::main_algorithm_name = "PWDefault";
std::vector<std::string> Parameters::ensemble_algorithms_names = {"PWDefaultRelaxDom","PWDefaultRelaxQueue"};
bool Parameters::use_ensemble = false;

/**Problem parameters**/
int Parameters::memory_threshold = 10000;

//Override Problem Data
int Parameters::origin = -1;
int Parameters::destination = -1;
int Parameters::ub_critical = -1;

//Scaling
bool Parameters::scaling_override = false;
float Parameters::scaling_value = 100.0;
std::string Parameters::scaling_target = "objective";

//Coordinates
float Parameters::coord_scaling = 1.0;
int Parameters::coord_distance_type = DIST_NONE;
float Parameters::coord_distance_scaling = 0.95;

/**Preprocessing type**/
bool Parameters::preprocessing_critical = false;

/**Default Algorithm (PWDefault) Parameters**/
bool Parameters::default_autoconfig = true;
float Parameters::default_timelimit = 0;
bool Parameters::default_parallel = true;
bool Parameters::default_bidirectional = true;
double Parameters::default_split = 0.5;
int Parameters::default_reserve = 10000000;

bool Parameters::default_use_visited = true;
bool Parameters::default_compare_unreachables = true;
int Parameters::default_dssr = 1;
int Parameters::default_ng = 0;
int Parameters::default_ng_size = 8;

int Parameters::default_candidate_type = 1;
int Parameters::default_join_type = 2;
bool Parameters::default_earlyjoin = false;
unsigned long long int Parameters::default_earlyjoin_step = 300;

int Parameters::default_queue_limit = 10;

/**Data Collection**/
bool Parameters::output_write = false;
std::string Parameters::timestamp = "YYYY-MM-DD_hh:mm:ss";
int Parameters::collection_level = -1;
std::string Parameters::collection_folder = "output/";
std::string Parameters::collection_tag;
std::string Parameters::collection_path;

void Parameters::readParameters(std::string param_path) {
    std::ifstream f;
    std::string command, value;
    char separator;

    f.open(param_path.empty() ? Parameters::param_path : param_path);
    if(f) {
        while(f >> command >> separator >> value) {
            if(command == "verbosity")
                verbosity = stoi(value);
            else if(command == "main_algorithm")
                main_algorithm_name = value;
            else if(command == "ensemble/algorithms"){
                ensemble_algorithms_names.clear();
                std::string name;
                std::stringstream value_stream(value);
                while (std::getline(value_stream, name, ','))
                    ensemble_algorithms_names.emplace_back(name);
            }
            else if(command == "ensemble/active")
                use_ensemble = stoi(value);
            else if(command == "problem/coordinates/distance") {
                if(value == "euclidean")
                    coord_distance_type = DIST_EUCLIDEAN;
                else if(value == "haversine")
                    coord_distance_type = DIST_HAVERSINE;
                else if(value == "equirectangular")
                    coord_distance_type = DIST_EQUIRECTANGULAR;
            }
            else if(command == "problem/coordinates/distance/scaling")
                coord_distance_scaling = stof(value);
            else if(command == "problem/coordinates/microdegrees") {
                if(stof(value) > 0)
                    coord_scaling = 1/1e6;
            }
            else if(command == "problem/memory_threshold")
                memory_threshold = stoi(value);
            else if(command == "problem/scaling/override")
                scaling_override = stoi(value);
            else if(command == "problem/scaling"){
                scaling_value = stof(value);
            }
            else if(command == "problem/scaling/target")
                scaling_target = value;
            else if(command == "algorithm/preprocessing/critical")
                preprocessing_critical = stoi(value);
            else if(command == "algo/default/autoconfig")
                default_autoconfig = stoi(value);
            else if(command == "algo/default/timelimit")
                default_timelimit = stof(value);
            else if(command == "algo/default/parallel")
                default_parallel = stoi(value);
            else if(command == "algo/default/bidirectional")
                default_bidirectional = stoi(value);
            else if(command == "algo/default/bidirectional/split")
                default_split = stof(value);
            else if(command == "algo/default/reserve")
                default_reserve = stoi(value);
            else if(command == "algo/default/use_visited")
                default_use_visited= stoi(value);
            else if(command == "algo/default/compare_unreachables")
                default_compare_unreachables = stoi(value);
            else if(command == "algo/default/dssr") {
                if(value ==  "off")
                    default_dssr = DSSR_OFF;
                else if(value == "restricted")
                    default_dssr = DSSR_RESTRICTED;
                else
                    default_dssr = DSSR_STANDARD;
            }
            else if(command == "algo/default/ng") {
                if(value ==  "off")
                    default_ng = NG_OFF;
                else if(value == "restricted")
                    default_ng = NG_RESTRICTED;
                else
                    default_ng = NG_STANDARD;
            }
            else if(command == "algo/default/ng/set_size")
                default_ng_size = stoi(value);
            else if(command == "algo/default/candidate/type") {
                if(value == "round-robin")
                    default_candidate_type = CANDIDATE_RR;
                else
                    default_candidate_type = CANDIDATE_NODE;
            }
            else if(command == "algo/default/join/type") {
                if(value == "classic")
                    default_join_type = JOIN_CLASSIC;
                else if(value == "naive")
                    default_join_type = JOIN_NAIVE;
                else
                    default_join_type = JOIN_ORDERED;
            }
            else if(command == "algo/default/join/early")
                default_earlyjoin = stoi(value);
            else if(command == "algo/default/join/early/step")
                default_earlyjoin_step = stoi(value);
            else if(command == "algo/default/relaxations/queue_limit")
                default_queue_limit = stoi(value);
            else if(command == "data_collection/level")
                collection_level = stoi(value);
            else if(command == "data_collection/tag") {
                if(collection_tag.empty())
                    collection_tag = value;
            }
            else if(command == "output/write")
                output_write = stoi(value);
        }
        f.close();
    }
    else std::cout<<"Warning: No param file found!"<<std::endl;

    if(not isCollecting())
        output_write = false;
}

//Output path setup
void Parameters::setupCollectionPath(){
    collection_path = collection_folder + "/" + timestamp;
    collection_path += (collection_tag != "" ? "[" + collection_tag + "]" : "");

    int name_count = 1;
    std::string tmp_path = collection_path;

    while(std::filesystem::exists(tmp_path)){
        tmp_path = collection_path + "_" + std::to_string(name_count);
        name_count++;
    }

    collection_path = tmp_path + "/";
}


bool Parameters::parseConsole(int argc, char **argv) {
    bool success = false;

    if(argc <= 1) {
        std::cout<<"Instance file is missing...terminating."<<std::endl;
        return success;
    }

    instance_path = argv[1];
    success = true;

    int pos = 2;
    std::string command;
    while(pos < argc) {
        command = argv[pos++];
        if(pos  == argc) {
            std::cout<<"Missing value for parameter: " << command <<", terminating..." <<std::endl;
            return success = false;
        }
        if(command == "-o")
            origin = atoi(argv[pos++]);
        else if(command == "-d")
            destination = atoi(argv[pos++]);
        else if(command == "-ub")
            ub_critical = atoi(argv[pos++]);
        else if(command == "-param")
            param_path = argv[pos++];
        else if(command == "-tag")
            collection_tag = argv[pos++];
        else {
            std::cout<<"Parameter: " << command << " not recognized, terminating..." <<std::endl;
            return success = false;
        }

    }

    return success;
}