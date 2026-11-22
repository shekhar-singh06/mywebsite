#ifndef VEHICLEHASHMAP_H
#define VEHICLEHASHMAP_H

#include <string>
#include <unordered_map>

class VehicleHashMap
{
private:
    std::unordered_map<std::string, std::string> vehicleMap;

public:
    void insert(std::string plate, std::string location);
    bool exists(std::string plate);
    std::string getLocation(std::string plate);
    void remove(std::string plate);
};

#endif