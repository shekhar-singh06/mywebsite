#include "ParkingLot.h"

ParkingLot::ParkingLot()
{
}

string ParkingLot::getStatus()
{
    string json = "{";
    json += "\"regular\":" + slots.getRegularJson() + ",";
    json += "\"vip\":" + slots.getVIPJson() + ",";
    json += "\"queue\":" + waitingqueue.getQueueJson() + ",";
    json += "\"stackSize\":" + to_string(undoStack.size());
    json += "}";
    return json;
}

string ParkingLot::vehicleEntry(string plate, string type)
{
    return VehicleManager::handleEntry(plate, type, slots, waitingqueue, vehicleMap, undoStack);
}

string ParkingLot::vehicleExit(string plate)
{
    return VehicleManager::handleExit(plate, slots, waitingqueue, vehicleMap, undoStack);
}

string ParkingLot::undoLastAction()
{
    return undoStack.undoLastAction(slots, waitingqueue, vehicleMap);
}

string ParkingLot::searchVehicle(string plate)
{
    if (vehicleMap.exists(plate))
    {
        string location = vehicleMap.getLocation(plate);
        return "{\"found\":true,\"location\":\"" + location + "\"}";
    }

    return "{\"found\":false,\"location\":\"\"}";
}

size_t ParkingLot::getStackSize()
{
    return undoStack.size();
}