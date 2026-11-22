#include "VehicleHashMap.h"

using namespace std;

// Add vehicle
void VehicleHashMap::insert(string plate, string location)
{
    vehicleMap[plate] = location;
}

bool VehicleHashMap::exists(string plate)
{
    if (vehicleMap.find(plate) != vehicleMap.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

string VehicleHashMap::getLocation(string plate)
{
    if (exists(plate))
    {
        return vehicleMap[plate];
    }
    else
    {
        return "Not Found";
    }
}

void VehicleHashMap::remove(string plate)
{
    if (exists(plate))
    {
        vehicleMap.erase(plate);
    }
}