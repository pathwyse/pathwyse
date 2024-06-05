#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "param.h"
#include <chrono>
#include <unordered_map>
#include <list>
#include <iostream>

class DataCollector {

public:

    /** Data Collector Management **/

    //Constructors and destructors
    DataCollector() = default;
    explicit DataCollector(std::string collection_name): name(collection_name) {setOutputPath(name);};
    ~DataCollector() = default;

    void setCollectionName(std::string name);
    void setOutputPath(std::string name) {output_path = Parameters::getCollectionPath() + name + ".csv";}

    //Dictionary management
    int addDictionary(std::string & tag, int type);
    int findIndex(std::string & tag, int type);
    std::string getValue(std::string & tag);            //Get a value (from tag), as a string
    std::string getDataStructure(std::string & tag);

    /** Initialize collection of a value **/

    //init (single value)
    void init(std::string tag, int value);
    void init(std::string tag, std::string value);
    void init(std::string tag, double value);
    void initTime(std::string tag, double value = 0);

    //init (multiple values)
    void init(std::vector<std::string> tags, std::vector<int> values);
    void init(std::vector<std::string> tags, std::vector<std::string> values);
    void init(std::vector<std::string> tags, std::vector<double> values);
    void initTimes(std::vector<std::string> & tags);

    /** Collect and increment a value **/

    //Collect (single value)
    void collect(std::string tag, int value);
    void collect(std::string tag, std::string value);
    void collect(std::string tag, double value);

    //Collect (multiple values)
    void collect(std::vector<std::string> tags, std::vector<int> values);
    void collect(std::vector<std::string> tags, std::vector<std::string> values);
    void collect(std::vector<std::string> tags, std::vector<double> values);

    //Increment values
    void increment(std::string tag, int value);
    void increment(std::string tag, double value);

    /** Time profile management **/

    void startTime(std::string tag);
    void stopTime(std::string tag);
    void startGlobalTime();
    void stopGlobalTime();
    double getGlobalTime() {return times_cumulative[0];}
    double getGlobalTimeNow();

    /** Output management **/
    void setHeader();
    void saveRecord();
    void writeData();
    void print();
    void print(std::vector<std::string> tags);

    /** Reset values and clear data structures **/
    void reset(std::vector<std::string> tags);

    void resetText(int id) {text[id] = "";}
    void resetTime(std::string tag, double value = 0);

    void resetTexts() {std::fill(text.begin(), text.end(), "");}
    void resetTimes(double value = 0) {std::fill(times.begin(), times.end(), value);}
    void resetTimesCumulative(double value = 0) {std::fill(times_cumulative.begin(), times_cumulative.end(), value);}
    void resetIntegers(){ std::fill(integers.begin(), integers.end(), 0);}
    void resetDecimals(){std::fill(decimals.begin(), decimals.end(), 0);}

    void clearRecords() {records.clear();}
    void clear();

    /** Direct access to Data **/
    //For best performance

    //Text
    void setText(int id, std::string text) {this->text[id] = text;}
    std::string getText(int id) {return text[id];}

    //Times
    void startTime(int id){time_point[id] = std::chrono::high_resolution_clock::now();}
    void stopTime(int id);
    double getTime(int id) {return times[id];}
    double getTimeCumulative(int id) {return times_cumulative[id];}
    void resetTime(int id) {times[id] = 0;}
    void resetTimeCumulative(int id){times_cumulative[id] = 0;}

    //Integers
    void setInteger(int id, int value){this->integers[id] = value;}
    void incrementInteger(int id, int value) {integers[id] += value;}
    int getInt(int id) {return integers[id];}
    void resetInteger(int id) {integers[id] = 0;}

    //Decimals
    void setDecimal(int id, double value){decimals[id] = value;}
    void incrementDecimal(int id, double value) {decimals[id] += value;}
    double getDecimal(int id) {return decimals[id];}
    void resetDecimal(int id) {decimals[id] = 0;}

private:

    //Profiling
    std::string name;
    std::string output_path;

    //Dictionary:
    //Tag, pair(data structure type, index)
    //Data structure types: see constants
    std::unordered_map<std::string, std::pair<int, int>> dict;

    //Text
    std::vector<std::string> tag_text;
    std::vector<std::string> text;

    //Times
    std::vector<double> times, times_cumulative;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> time_point;
    std::vector<std::string> tag_times;

    //Integers
    std::vector<int> integers;
    std::vector<std::string> tag_integers;

    //Decimals
    std::vector<double> decimals;
    std::vector<std::string> tag_decimals;

    //Data collection
    std::string header;
    std::list<std::string> records;
};

#endif