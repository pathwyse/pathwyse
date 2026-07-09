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
    static std::vector<std::string>& getAlgoNames(){return algo_names;}
    static std::vector<bool>& getAlgoActive(){return algo_active;}
    static bool areAlgoParallel(){return algo_parallel;}

    /**Problem parameters**/
    static int getCompressionThreshold(){return memory_threshold;}

    //Override Problem Data
    bool static parseConsole(int argc, char **argv);
    static int getOrigin() {return origin;}
    static int getDestination() {return destination;}
    static int getCriticalUB(int id) {return id < ub_resources.size() ? ub_resources[id] : -1;}

    //Decimal digits after comma requested by user for cost scaling
    static int getDecimalDigits() {return decimal_digits;}

    //Coordinates
    static float getCoordScaling(){return coord_scaling;}
    static float getCoordDistanceScaling(){return coord_distance_scaling;}

    /**Preprocessing algorithm**/
    static int getPreprocessingIntensity(){return preprocessing_intensity;}
    static bool isPreprocessingCritical(){return preprocessing_critical;}

    /**Default Algorithm**/
    static bool isDefaultAutoConfigured() {return default_autoconfig;}
    static float getDefaultTimelimit(){return default_timelimit;}
    static bool isDefaultParallel() {return default_parallel;}
    static int getDefaultSearch() {return default_search;}
    static double getDefaultSplit() {return default_split;}
    static int getDefaultReserve() {return default_reserve;}

    static bool isDefaultUsingVisited(){return default_use_visited;}
    static bool isDefaultUsingUnreachables(){return default_compare_unreachables;}
    static int getDefaultDSSR(){return default_dssr;}
    static int getDefaultNG(){return default_ng;}
    static int getDefaultNGSize(){return default_ng_size;}
    static std::string getDefaultNGSetPath(){return default_ng_set_path;}

    static int getDefaultRelaxationQueueLimit(){return default_queue_limit;}

    static int getDefaultCandidateType(){return default_candidate_type;}
    static int getDefaultJoinType(){return default_join_type;}
    static bool isDefaultJoinEarly() {return default_earlyjoin;}
    static unsigned long long int getDefaultJoinStep() {return default_earlyjoin_step;}
    static int getRequestedSolutions(){return requested_solutions;}

    static bool useTwoCycleElimination(){return two_cycle_elimination;}

    /** Bucket algorithm **/
    static int getBucketResource(){return bucket_resource;}
    static std::string getBucketSizeMode(){return bucket_size_mode;}
    static float getBucketPrevDomCheckPercentage(){return bucket_prev_percentage;}
    static float getBucketNextDomCheckPercentage(){return bucket_next_percentage;}

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
    static std::vector<std::string> algo_names;
    static std::vector<bool> algo_active;
    static bool algo_parallel;

    /**Problem parameters**/
    static int memory_threshold;    //Global parameter

    //Problem data override
    static int origin;              //Override parameter
    static int destination;         //Override parameter
    static std::vector<int> ub_resources;         //Override parameter

    //Decimal digits
    static int decimal_digits;

    //Coordinates
    static float coord_scaling;
    static float coord_distance_scaling;

    /**Preprocessing**/
    static int preprocessing_intensity;
    static bool preprocessing_critical;

    /**Default Algorithm (PWDefault) Parameters**/
    static int requested_solutions;
    static float default_timelimit;
    static bool default_parallel;
    static int default_search;
    static bool default_use_visited;
    static int default_dssr;
    static int default_ng;
    static int default_ng_size;
    static std::string default_ng_set_path;
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

    static bool two_cycle_elimination;

    /**Bucket Algorithm (PWBucket) Parameters**/
    static int bucket_resource;
    static std::string bucket_size_mode;
    static float bucket_prev_percentage;
    static float bucket_next_percentage;

    /**Data Collection**/
    static bool output_write;                                               //Global parameter
    static std::string timestamp;                                           //Global parameter
    static int collection_level;                                            //Global parameter
    static std::string collection_folder, collection_tag, collection_path;  //Global parameter
};

#endif //SPPRCLIB_PARAM_H
