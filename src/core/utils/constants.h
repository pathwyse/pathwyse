#include <limits>

/** Status **/

//Problem status
#define PROBLEM_INDETERMINATE 0
#define PROBLEM_FEASIBLE 1
#define PROBLEM_INFEASIBLE 2

//Solver status
#define SOLVER_START 0
#define SOLVER_READY 1
#define SOLVER_BUSY 2

//Algorithm status
#define ALGO_READY 0
#define ALGO_OPTIMIZING 1
#define ALGO_DONE 2
#define ALGO_BOUNDLIMIT 3
#define ALGO_GAPLIMIT 4
#define ALGO_TIMELIMIT 5

//Solution status
#define PATH_UNKNOWN 0
#define PATH_OPTIMAL 1
#define PATH_FEASIBLE 2
#define PATH_SUPEROPTIMAL 3
#define PATH_INFEASIBLE 4

/** Solution **/

//Solution ranking selection
#define RANK_OBJECTIVE ("Objective")

//Solution values
#define UNKNOWN std::numeric_limits<int>::max()
#define INFPLUS std::numeric_limits<int>::max()
#define INFMINUS std::numeric_limits<int>::min()

/** Problem **/

//Resource type creation
#define RES_CAPACITY 0
#define RES_TIME 1
#define RES_TIMEWINDOWS 2
#define RES_NODELIM 3

//Resource selection
#define RES_COST (-1)
#define RES_CRITICAL 0

//Distance type selection
#define DIST_NONE 0
#define DIST_EUCLIDEAN 1
#define DIST_HAVERSINE 2
#define DIST_EQUIRECTANGULAR 3

//Numerical values
#define MAX_JOIN_STEP 4294967296
#define EARTH_RADIUS 6371008.8

/** Algorithm **/

//Algorithm selection
#define MAIN_ALGORITHM (-1)

//Algorithm type
#define ALGO_EXACT ("Exact")
#define ALGO_HEURISTIC ("Heuristic")

//Bidirectional algorithm direction
#define FORWARD 1
#define BACKWARD 0

//Relaxation mode selection
#define DSSR_OFF 0
#define DSSR_STANDARD 1
#define DSSR_RESTRICTED 2
#define NG_OFF 0
#define NG_STANDARD 1
#define NG_RESTRICTED 2

//Candidate Types
#define CANDIDATE_RR 0
#define CANDIDATE_NODE 1

//Join Types
#define JOIN_NAIVE 0
#define JOIN_CLASSIC 1
#define JOIN_ORDERED 2

/** Data collection **/

//Data type selection
#define TEXT 0
#define INTEGER 1
#define DECIMALS 2
#define TIME 3

//Data collection index
#define T_INS_FW 4
#define T_INS_BW 5

/** Utilities **/
#define EPS 10E-7

