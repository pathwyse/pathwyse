#include "data_collector.h"

/** Data Collector Management **/

void DataCollector::setCollectionName(std::string name) {
    this->name = name;
    setOutputPath(name);
}

//Manage unordered map
int DataCollector::addDictionary(std::string & tag, int type) {
    int index = 0;

    switch(type) {
        case INTEGER:
            tag_integers.push_back(tag);
        integers.push_back(0);
        index = integers.size() - 1;
        break;
        case DECIMALS:
            tag_decimals.push_back(tag);
        decimals.push_back(0);
        index = decimals.size() - 1;
        break;
        case TIME:
            tag_times.push_back(tag);
        times.push_back(0);
        times_cumulative.push_back(0);
        time_point.push_back(std::chrono::high_resolution_clock::now());
        index = times.size() - 1;
        break;
        default:
            tag_text.push_back(tag);
        text.emplace_back("");
        index = text.size() - 1;
        break;
    }

    dict.insert({tag, std::make_pair(type, index)});
    return index;
}


int DataCollector::findIndex(std::string & tag, int type) {
    int index = -1;

    if(Parameters::isCollecting()) {
        auto position = dict.find(tag);
        if(position != dict.end())
            index = position->second.second;
    }
    return index;
}

std::string DataCollector::getValue(std::string & tag){
    std::string value = "NULL";
    auto pos = dict.find(tag);

    if(pos != dict.end()) {
        int type = pos->second.first;
        int index = pos->second.second;

        switch(type) {
            case INTEGER:
                value = std::to_string(integers[index]);
            break;
            case DECIMALS:
                value = std::to_string(decimals[index]);
            break;
            case TIME:
                value = std::to_string(times[index]);
            break;
            default:
                value = text[index];
            break;
        }
    }

    return value;
}

std::string DataCollector::getDataStructure(std::string &tag) {
    auto pos = dict.find(tag);

    if(pos != dict.end()) {
        int type = pos->second.first;

        switch(type) {
            case INTEGER:
                return "1) integer";
            case DECIMALS:
                return "2) decimal";
            case TIME:
                return "3) time";
            default:
                return "0) text";
        }
    }

    return "not found";
}


/** Initialize collection of a value **/

//Inits (single value)
void DataCollector::init(std::string tag, std::string value) {
    int index = addDictionary(tag, TEXT);
    text[index] = std::move(value);
}

void DataCollector::init(std::string tag, int value){
    int index = addDictionary(tag, INTEGER);
    integers[index] = value;
}

void DataCollector::init(std::string tag, double value){
    int index = addDictionary(tag, DECIMALS);
    decimals[index] = value;
}

void DataCollector::initTime(std::string tag, double value) {
    int index = addDictionary(tag, TIME);
    times[index] = value;
    times_cumulative[index] = value;
}

//Inits (multiple values)
void DataCollector::init(std::vector<std::string> tags, std::vector<std::string> values) {
    for(int i = 0; i < tags.size(); i++)
        init(tags[i], values[i]);
}
void DataCollector::init(std::vector<std::string> tags, std::vector<int> values) {
    for(int i = 0; i < tags.size(); i++)
        init(tags[i], values[i]);
}
void DataCollector::init(std::vector<std::string> tags, std::vector<double> values) {
    for(int i = 0; i < tags.size(); i++)
        init(tags[i], values[i]);
}

void DataCollector::initTimes(std::vector<std::string> & tags) {
    for(auto & t: tags)
        initTime(t);
}

/** Collect and increment a value **/

//Collect (single value)
void DataCollector::collect(std::string tag, std::string value){
    int index = findIndex(tag, TEXT);
    if(index >= 0)
        text[index] = std::move(value);
}

void DataCollector::collect(std::string tag, int value){
    int index = findIndex(tag, INTEGER);
    if(index >= 0)
        integers[index] = value;
}

void DataCollector::collect(std::string tag, double value){
    int index = findIndex(tag, DECIMALS);
    if(index >= 0)
        decimals[index] = value;
}

//Collect (multiple values)
void DataCollector::collect(std::vector<std::string> tags, std::vector<std::string> values) {
    for(int i = 0; i < tags.size(); i++)
        collect(tags[i], values[i]);
}

void DataCollector::collect(std::vector<std::string> tags, std::vector<int> values) {
    for(int i = 0; i < tags.size(); i++)
        collect(tags[i], values[i]);
}

void DataCollector::collect(std::vector<std::string> tags, std::vector<double> values) {
    for(int i = 0; i < tags.size(); i++)
        collect(tags[i], values[i]);
}

//Increment values
void DataCollector::increment(std::string tag, int value) {
    int index = findIndex(tag, INTEGER);
    if(index >= 0)
        integers[index] += value;
}

void DataCollector::increment(std::string tag, double value) {
    int index = findIndex(tag, DECIMALS);
    if(index >= 0)
        decimals[index] += value;
}

/** Time management **/

void DataCollector::startTime(std::string tag) {
    int index = findIndex(tag, TIME);
    if(index >= 0)
        time_point[index] = std::chrono::high_resolution_clock::now();
}

void DataCollector::stopTime(std::string tag) {
    int index = findIndex(tag, TIME);
    if(index >= 0) {
        double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - time_point[index]).count();
        times[index] += time;
        times_cumulative[index] += time;
    }
}

void DataCollector::startGlobalTime() {
    time_point[0] = std::chrono::high_resolution_clock::now();
}

void DataCollector::stopGlobalTime() {
    double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - time_point[0]).count();
    times[0] += time;
    times_cumulative[0] += time;
}

double DataCollector::getGlobalTimeNow(){
    double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - time_point[0]).count();
    return times_cumulative[0] + time;
}


/** Output management **/

//Record Data
void DataCollector::setHeader(){
    if(not Parameters::isOutputStored())
        return;

    header = "";

    for(auto & t: tag_text)
        header += t + ",";


    if(not tag_times.empty()) {
        header += tag_times[0] + ",global_time,";

        for(int i = 1; i < tag_times.size(); i++)
            header += tag_times[i] + "," + tag_times[i]  + "_cumulative,";
    }


    for(auto & t: tag_integers)
        header += t + ",";

    for(auto & t: tag_decimals)
        header += t + ",";

    //Manage and save header end
    if(header != "") {
        header.resize(header.size() - 1);
        header += "\n";
    }
}

void DataCollector::saveRecord() {
    if(not Parameters::isOutputStored())
        return;

    std::string r;
    for(auto & t: text)
        r += t + ",";
    for(int t = 0; t < times.size(); t++)
        r += std::to_string(times[t]) + "," + std::to_string(times_cumulative[t]) + ",";
    for(auto & i: integers)
        r += std::to_string(i) + ",";
    for(auto & d: decimals)
        r += std::to_string(d) + ",";

    //Manage and save record
    if(r != ""){
        r.resize(r.size() - 1);       //removes the last ","
        r += "\n";                       //adds endline
        records.push_back(r);
    }

}

void DataCollector::writeData() {
    if(not Parameters::isOutputStored() or records.empty())
        return;

    //Check if file already exists
    bool write_header = true;
    std::ifstream f_in(output_path.c_str());
    if(f_in.good()) write_header = false;
    f_in.close();

    //Save header and records to csv
    std::ofstream f;
    f.open(output_path.c_str(), std::ios_base::app);
    if(write_header) f << header;
    for (auto &r: records) f << r;
    f.close();
}


void DataCollector::print(){
    if(not Parameters::isCollecting() or Parameters::getVerbosity() < 3)
        return;

    std::cout<<"-------Printing collection-------"<<std::endl;
    std::cout<<"Collection: " << name << std::endl;
    std::string line;
    for(int i = 0; i < tag_text.size(); i++){
        line = tag_text[i] + ": " + text[i];
        std::cout<<line<<std::endl;
    }

    for(int i = 0; i < tag_times.size(); i++){

        line = tag_times[i] + ": " + std::to_string(times[i]);
        std::cout<<line<<std::endl;
        if(i > 0)
            line = tag_times[i] + "_cumulative: " + std::to_string(times_cumulative[i]);
        else
            line = "Global_time: " + std::to_string(times_cumulative[i]);
        std::cout<<line<<std::endl;
    }

    for(int i = 0; i < tag_integers.size(); i++){
        line = tag_integers[i] + ": " + std::to_string(integers[i]);
        std::cout<<line<<std::endl;
    }

    for(int i = 0; i < tag_decimals.size(); i++){
        line = tag_decimals[i] + ": " + std::to_string(decimals[i]);
        std::cout<<line<<std::endl;
    }

    std::cout<<"--------------------"<<std::endl;
}

void DataCollector::print(std::vector<std::string> tags){
    if(not Parameters::isCollecting() or Parameters::getVerbosity() < 0)
        return;

    std::string value;
    int type;

    for(auto & t: tags) {

        if(t == "Global_time") {
            std::cout<<"Global_time: " << getGlobalTime() <<std::endl;
            continue;
        }

        std::cout<<t + ": ";
        auto pos = dict.find(t);

        if(pos != dict.end()) {
            int type = pos->second.first;
            int index = pos->second.second;

            switch(type) {
                case INTEGER:
                    std::cout<<getInt(index);
                    break;
                case DECIMALS:
                    std::cout<<getDecimal(index);
                    break;
                case TIME:
                    std::cout<<getTime(index)<<std::endl;
                    std::cout<<t + "_cumulative: " << getTimeCumulative(index);
                    break;
                default:
                    std::cout<<getText(index);
                    break;
            }
        }

        std::cout<<std::endl;
    }

}

/** Reset values and clear data structures **/

void DataCollector::reset(std::vector<std::string> tags) {
    if(not Parameters::isCollecting())
        return;

    for(auto & tag: tags) {
        auto pos = dict.find(tag);

        if(pos != dict.end()) {
            int type = pos->second.first;
            int index = pos->second.second;

            switch(type) {
                case INTEGER:
                    resetInteger(index);
                    break;
                case DECIMALS:
                    resetDecimal(index);
                    break;
                case TIME:
                    resetTime(index);
                    break;
                default:
                    resetText(index);
                    break;
            }
        }
    }
}

void DataCollector::resetTime(std::string tag, double value) {
    int index = findIndex(tag, TIME);
    if(index >= 0)
        times[index] = value;
}

void DataCollector::clear() {
    dict.clear();

    text.clear();
    times.clear();
    times_cumulative.clear();
    integers.clear();
    decimals.clear();

    tag_text.clear();
    tag_times.clear();
    tag_integers.clear();
    tag_decimals.clear();
}

/** Direct access to Data **/

void DataCollector::stopTime(int id) {
    double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - time_point[id]).count();
    times[id] += time;
    times_cumulative[id] += time;
}