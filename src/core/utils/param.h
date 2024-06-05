#ifndef SPPRCLIB_PARAM_H
#define SPPRCLIB_PARAM_H

#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include "constants.h"


struct Parameters {

    static void readParameters(std::string param_path);

    /**Solver Parameters**/
    static std::string getInstancePath(){return instance_path;}
    static int getVerbosity() {return verbosity;}
    static void setVerbosity(int v) {verbosity = v;}

    //Algorithm selection and targets
    static std::string getMainAlgorithmName(){return main_algorithm_name;}
    static std::vector<std::string> & getEnsembleNames(){return ensemble_algorithms_names;}
    static bool isEnsembleUsed(){return use_ensemble;}

    /**Problem parameters**/
    static int getCompressionThreshold(){return memory_threshold;}

    //Override Problem Data
    bool static parseConsole(int argc, char **argv);
    static int getOrigin() {return origin;}
    static int getDestination() {return destination;}
    static int getCriticalUB() {return ub_critical;}

    //Scaling
    bool static isScalingOverridden(){return scaling_override;}
    float static getScaling(){return scaling_value;}
    std::string static getScalingTarget(){return scaling_target;}

    //Coordinates
    static float getCoordScaling(){return coord_scaling;}
    static int getCoordDistanceType(){return coord_distance_type;}
    static float getCoordDistanceScaling(){return coord_distance_scaling;}

    /**Preprocessing algorithm**/
    static bool isPreprocessingCritical(){return preprocessing_critical;}

    /**Default Algorithm**/
    static bool isDefaultAutoConfigured() {return default_autoconfig;}
    static float getDefaultTimelimit(){return default_timelimit;}
    static bool isDefaultParallel() {return default_parallel;}
    static bool isDefaultBidirectional() {return default_bidirectional;}
    static double getDefaultSplit() {return default_split;}
    static int getDefaultReserve() {return default_reserve;}

    static bool isDefaultUsingVisited(){return default_use_visited;}
    static bool isDefaultUsingUnreachables(){return default_compare_unreachables;}
    static int getDefaultDSSR(){return default_dssr;}
    static int getDefaultNG(){return default_ng;}
    static int getDefaultNGSize(){return default_ng_size;}

    static int getDefaultRelaxationQueueLimit(){return default_queue_limit;}

    static int getDefaultCandidateType(){return default_candidate_type;}
    static int getDefaultJoinType(){return default_join_type;}
    static bool isDefaultJoinEarly() {return default_earlyjoin;}
    static unsigned long long int getDefaultJoinStep() {return default_earlyjoin_step;}

    /**Data Collection**/
    static bool isOutputStored(){return output_write;}
    static bool isCollecting(){return collection_level >= 0;}
    static int getCollectionLevel(){return collection_level;}
    static std::string getCollectionOutput() {return collection_folder;}
    static std::string getCollectionTag(){return collection_tag;}
    static void setCollectionTag(std::string tag){collection_tag = tag;}
    static std::string getTimestamp() {return timestamp;}
    static std::string getCollectionPath(){return collection_path;}
    static void setTimestamp(std::string t) {timestamp = t;}
    static void setupCollectionPath();

private:
    static std::string param_path;                              //Global parameter

    /**Solver Parameters**/
    static std::string instance_path;                           //Global parameter
    static int verbosity;                                       //Global parameter
    static std::string main_algorithm_name;
    static std::vector<std::string> ensemble_algorithms_names;
    static bool use_ensemble;

    /**Problem parameters**/
    static int memory_threshold;    //Global parameter

    //Problem data override
    static int origin;              //Override parameter
    static int destination;         //Override parameter
    static int ub_critical;         //Override parameter

    //Scaling
    static bool scaling_override;
    static float scaling_value;
    static std::string scaling_target;

    //Coordinates
    static float coord_scaling;
    static int coord_distance_type;
    static float coord_distance_scaling;

    /**Preprocessing type**/
    static bool preprocessing_critical;

    /**Default Algorithm (PWDefault) Parameters**/
    static float default_timelimit;
    static bool default_parallel;
    static bool default_bidirectional;
    static bool default_use_visited;
    static int default_dssr;
    static int default_ng;
    static int default_ng_size;
    static bool default_earlyjoin;
    static unsigned long long int default_earlyjoin_step;

    //Label Manager
    static bool default_autoconfig;
    static double default_split;
    static int default_reserve;
    static bool default_compare_unreachables;
    static int default_candidate_type;
    static int default_join_type;

    //Default algorithm relaxations parameters
    static int default_queue_limit;

    /**Data Collection**/
    static bool output_write;                                               //Global parameter
    static std::string timestamp;                                           //Global parameter
    static int collection_level;                                            //Global parameter
    static std::string collection_folder, collection_tag, collection_path;  //Global parameter
};

#endif //SPPRCLIB_PARAM_H
