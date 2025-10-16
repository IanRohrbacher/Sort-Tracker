#include <iostream>
#include <cstdlib>
#include <fstream>
#include <deque>
#include <algorithm>
#include <random>
#include <thread>
#include <iomanip>
#include <chrono>
#include <nlohmann/json.hpp>
#include "sortAlgorithm.hpp"

using json = nlohmann::json;
using String = std::string;

void createJson(String jsonName);
void createBackup(String jsonName);
void loadBackup(String jsonName);

// getters
unsigned long long getLastListSize(const json& jsonData);
unsigned long long getInteraction(const json& jsonData, unsigned long long listSize);

// modifiers
void loadJson(String jsonName, json& jsonData);
bool sortJsonData(String jsonName, json& jsonData);

void createNewList(String jsonName, json& jsonData, unsigned long long listSize);

void createNewInteractionEntry(String jsonName, json& jsonData, unsigned long long listSize);
void updateLastInteraction(String jsonName, json& jsonData);

void addEndTime(String jsonName, json& jsonData, unsigned long long listSize);
String getTime();

// printers
void printLatestInteraction(const json& jsonData, std::ostream& out);
void printAllInteractions(const json& jsonData, std::ostream& out);

// terminal text colors
const String RED_TEXT = "\033[1;31m";
const String GREEN_TEXT = "\033[1;32m";
const String RESET_TEXT = "\033[0m";

int main(int argc, char* argv[]) {
    // Check if the number of arguments is correct
    if(argc == 2) {
        std::cerr << GREEN_TEXT << "json reset" << RESET_TEXT << std::endl;
        createJson(argv[1]);
        return 0;
    }
    if(argc != 6) {
        std::cerr << RED_TEXT << "Invalid number of arguments" << RESET_TEXT << std::endl;
        return 0;
    }

    // creates a custom output stream
    std::ostream* out;
    std::ofstream fileOut;
    bool outputLocation = ("true" == String(argv[1]));
    if(outputLocation) {
        fileOut.open("output.txt");
        out = &fileOut;
    } else {
        out = &std::cout;
    }
    bool willLoop = ("true" == String(argv[2]));

    // opens the json data
    String jsonName = argv[3];
    std::ifstream file(jsonName);
    if (!file.is_open()) {
        std::cerr << RED_TEXT << "Failed to open file" << RESET_TEXT << std::endl;
        return 1;
    }
    json jsonData;
    try {
        file >> jsonData;
    } catch (const json::parse_error& e) {
        std::cerr << RED_TEXT << "JSON parse error: " << e.what() << RESET_TEXT << std::endl;
        return 1;
    }
    file.close();

    // main script
    do {
        bool sorted = false;
        static unsigned long long listSize = getLastListSize(jsonData);

        if(listSize == -1) {
            std::cerr << RED_TEXT << "Failed to get the current listSize" << RESET_TEXT << std::endl;
            return 1;
        }
        createNewList(jsonName, jsonData, listSize);
        while(!sorted) {
            sorted = sortJsonData(jsonName, jsonData);
            updateLastInteraction(jsonName, jsonData);
            if((getInteraction(jsonData, listSize) % std::stoi(argv[5])) == 0) {
                createBackup(jsonName);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(argv[4])));
        }
        addEndTime(jsonName, jsonData, listSize);
        createNewInteractionEntry(jsonName, jsonData, ++listSize);
        createBackup(jsonName);
    } while(willLoop);

    return 0;
}

// writes the json data to the file
void loadJson(String jsonName, json& jsonData) {
    std::ofstream outFile(jsonName);
    if (outFile.is_open()) {
        outFile << jsonData.dump(4);
        outFile.close();
    } else {
        std::cerr << RED_TEXT << "Failed to open file for writing" << RESET_TEXT << std::endl;
    }
}

// creates a new list of listSizes from 1 to listSize, shuffles it, and writes it to the json
void createNewList(String jsonName, json& jsonData, unsigned long long listSize) {
    std::deque<int> list;
    for(unsigned long long i = 1; i <= listSize; i++) {
        list.push_back(i);
    }
    std::random_device rd; // Obtain a random listSize from hardware
    std::mt19937 rng(rd());
    std::shuffle(std::begin(list), std::end(list), rng);
    jsonData["list"] = list;
    loadJson(jsonName, jsonData);
}

// reads the json data, loads the list, modifys it, and writes it back to the json
bool sortJsonData(String jsonName, json& jsonData) {
    if (jsonData.contains("list") && jsonData["list"].is_array()) {
        std::deque<int> list = jsonData["list"].get<std::deque<int>>();
        sortTick(list);
        jsonData["list"] = list;
        loadJson(jsonName, jsonData);
        return std::is_sorted(list.begin(), list.end());
    } else {
        std::cerr << RED_TEXT << "'list' not found or is not an array" << RESET_TEXT << std::endl;
    }
    return false;
}


// pulls and returns the latest listSize from the json
unsigned long long getLastListSize(const json& jsonData) {
    if (jsonData.contains("list_storage") && jsonData["list_storage"].is_array()) {
        const auto& item = jsonData["list_storage"].back();
        if (item.contains("list_size") && item["list_size"].is_number()) {
            return item["list_size"].get<int>();
        } else {
            std::cerr << RED_TEXT << "Key 'listSize' not found or is not a listSize in the first item of 'listStorage'" << RESET_TEXT << std::endl;
        }
    } else {
        std::cerr << RED_TEXT << "'listStorage' is not found or is not an array" << RESET_TEXT << std::endl;
    }
    return -1;
}

// pulls and returns the listStorage of a listSize from the json
unsigned long long getInteraction(const json& jsonData, unsigned long long listSize) {
    if (jsonData.contains("list_storage") && jsonData["list_storage"].is_array()) {
        for (const auto& item : jsonData["list_storage"]) {
            if (item.contains("list_size") && item["list_size"].is_number()) {
                if (item["list_size"].get<int>() == listSize) {
                    if (item.contains("total_comparisons") && item["total_comparisons"].is_number()) {
                        return item["total_comparisons"].get<int>();
                    } else {
                        std::cerr << RED_TEXT << "Key 'total_comparisons' not found or is not a listSize in the item of 'listStorage'" << RESET_TEXT << std::endl;
                    }
                }
            } else {
                std::cerr << RED_TEXT << "Key 'listSize' not found or is not a listSize in the item of 'listStorage'" << RESET_TEXT << std::endl;
            }
        }
    } else {
        std::cerr << RED_TEXT << "'listStorage' is not found or is not an array" << RESET_TEXT << std::endl;
    }
    return -1;   
}

// prints the latest listStorage entry
void printLatestInteraction(const json& jsonData, std::ostream& out) {
    unsigned long long num = getLastListSize(jsonData);
    if(num != -1) {
        unsigned long long listStorage = getInteraction(jsonData, num);
        if(listStorage != -1) {
            out << "ListSize: " << num << ", Interaction: " << listStorage << std::endl;
        } else {
            out << RED_TEXT << "Failed to get the listStorage" << RESET_TEXT << std::endl;
        }
    } else {
        out << RED_TEXT << "Failed to get the current listSize" << RESET_TEXT << std::endl;
    }
}

// prints all the listStorage entries
void printAllInteractions(const json& jsonData, std::ostream& out) {
    unsigned long long num = getLastListSize(jsonData);
    if(num != -1) {
        for(unsigned long long i = 1; i <= num; i++) {
            unsigned long long listStorage = getInteraction(jsonData, i);
            if(listStorage != -1) {
                out << "ListSize: " << i << ", Interaction: " << listStorage << std::endl;
            } else {
                out << RED_TEXT << "Failed to get the listStorage" << RESET_TEXT << std::endl;
            }
        }
    } else {
        out << RED_TEXT << "Failed to get the current listSize" << RESET_TEXT << std::endl;
    }
}

// creates a new entry in the json data at the end with zero listStorage
void createNewInteractionEntry(String jsonName, json& jsonData, unsigned long long listSize) {
    json newEntry;
    newEntry["list_size"] = listSize;
    newEntry["total_comparisons"] = 0;
    newEntry["start_time"] = getTime();
    newEntry["end_time"] = "";
    jsonData["list_storage"].push_back(newEntry);

    loadJson(jsonName, jsonData);
}

// updates the last entry in the json data with the current total listStorage plus one
void updateLastInteraction(String jsonName, json& jsonData) {
    if (jsonData.contains("list_storage") && jsonData["list_storage"].is_array()) {
        auto& item = jsonData["list_storage"].back();
        if (item.contains("list_size") && item["list_size"].is_number()) {
            if (item.contains("total_comparisons") && item["total_comparisons"].is_number()) {
                unsigned long long total_comparisons = item["total_comparisons"].get<int>();
                jsonData["list_storage"].back()["total_comparisons"] = ++total_comparisons;

                loadJson(jsonName, jsonData);
            } else {
                std::cerr << RED_TEXT << "Key 'total_comparisons' not found or is not a listSize in the first item of 'listStorage'" << RESET_TEXT << std::endl;
            }
        } else {
            std::cerr << RED_TEXT << "Key 'listSize' not found or is not a listSize in the first item of 'listStorage'" << RESET_TEXT << std::endl;
        }
    } else {
        std::cerr << RED_TEXT << "'listStorage' is not found or is not an array" << RESET_TEXT << std::endl;
    }
}

void createJson(String jsonName) {
    json jsonData;
    jsonData["list"] = json::array();
    jsonData["list_storage"] = json::array({ { {"list_size", 1}, {"start_time", getTime()}, {"total_comparisons", 0}, {"end_time", ""} } });

    loadJson(jsonName, jsonData);
}

void createBackup(String jsonName) {
    std::ifstream file(jsonName);
    if (!file.is_open()) {
        std::cerr << RED_TEXT << "Failed to open file" << RESET_TEXT << std::endl;
        return;
    }
    json jsonData;
    try {
        file >> jsonData;
    } catch (const json::parse_error& e) {
        std::cerr << RED_TEXT << "JSON parse error: " << e.what() << RESET_TEXT << std::endl;
        return;
    }
    file.close();

    std::ofstream outFile(jsonName + ".bak");
    if (outFile.is_open()) {
        outFile << jsonData.dump(4);
        outFile.close();
    } else {
        std::cerr << RED_TEXT << "Failed to open file for writing" << RESET_TEXT << std::endl;
    }
}

void loadBackup(String jsonName) {
    std::ifstream file(jsonName + ".bak");
    if (!file.is_open()) {
        std::cerr << RED_TEXT << "Failed to open file" << RESET_TEXT << std::endl;
        return;
    }
    json jsonData;
    try {
        file >> jsonData;
    } catch (const json::parse_error& e) {
        std::cerr << RED_TEXT << "JSON parse error: " << e.what() << RESET_TEXT << std::endl;
        return;
    }
    file.close();

    loadJson(jsonName, jsonData);
}

// adds the end time to the last entry in the json data
void addEndTime(String jsonName, json& jsonData, unsigned long long listSize) {
    if (jsonData.contains("list_storage") && jsonData["list_storage"].is_array()) {
        auto& item = jsonData["list_storage"].back();
        if (item.contains("list_size") && item["list_size"].is_number()) {
            if (item.contains("end_time")) {
                jsonData["list_storage"].back()["end_time"] = getTime();
                loadJson(jsonName, jsonData);
            } else {
                std::cerr << RED_TEXT << "Key 'end_time' not found in the first item of 'listStorage'" << RESET_TEXT << std::endl;
            }
        } else {
            std::cerr << RED_TEXT << "Key 'listSize' not found or is not a listSize in the first item of 'listStorage'" << RESET_TEXT << std::endl;
        }
    } else {
        std::cerr << RED_TEXT << "'listStorage' is not found or is not an array" << RESET_TEXT << std::endl;
    }

}

// Get the current time in a specific format
String getTime() {
    // Get current time using system_clock
    auto now = std::chrono::system_clock::now();
    // Convert to time_t (seconds since epoch)
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    // Convert to local time structure
    std::tm local_tm = *std::localtime(&time_now);
    // Format and print the time with timezone    
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%b-%d-%Y %H:%M:%S %Z");
    return oss.str();
}