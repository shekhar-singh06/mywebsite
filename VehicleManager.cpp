#include "VehicleManager.h"
#include <regex>

string VehicleManager::handleEntry(string plate, string type, ParkingSlots& slots, waitingqueue& waitingQueue, VehicleHashMap& vehicleMap, UndoStack& undoStack)
{
    if (plate.empty())
    {
        return "{\"status\":\"error\",\"message\":\"License Plate is required!\"}";
    }

    std::regex platePattern("^[A-Z]{2}[0-9]{1,2}[A-Z]{0,2}[0-9]{4}$");
    
    if (!std::regex_match(plate, platePattern))
    {
        return "{\"status\":\"error\",\"message\":\"Invalid Indian License Plate format! (e.g., MH12AB1234)\"}";
    }

    if (vehicleMap.exists(plate))
    {
        return "{\"status\":\"error\",\"message\":\"Vehicle already registered or parked!\"}";
    }

    string allocated = "";
    ActionLog action;

    if (type == "VIP")
    {
        int index = slots.getFreeVIPSlot();
        if (index != -1)
        {
            slots.occupyVIP(index, plate);
            allocated = "VIP Slot " + to_string(index + 1);
            action = ActionLog(plate, "VIP", index);
        }
    }

    if (allocated == "")
    {
        int index = slots.getFreeRegularSlot();
        if (index != -1)
        {
            slots.occupyRegular(index, plate);
            allocated = "Regular Slot " + to_string(index + 1);
            action = ActionLog(plate, "REGULAR", index);
        }
    }

    if (allocated == "")
    {
        waitingQueue.push(plate);
        allocated = "Waiting Queue (FIFO)";
        action = ActionLog(plate, "QUEUE", -1);
    }

    vehicleMap.insert(plate, allocated);
    undoStack.push(action);

    return "{\"status\":\"success\",\"location\":\"" + allocated + "\"}";
}

string VehicleManager::handleExit(string plate, ParkingSlots& slots, waitingqueue& waitingQueue, VehicleHashMap& vehicleMap, UndoStack& undoStack)
{
    if (plate.empty() || !vehicleMap.exists(plate))
    {
        return "{\"status\":\"error\",\"message\":\"Vehicle not found in Parking Lot!\"}";
    }

    string loc = vehicleMap.getLocation(plate);
    vehicleMap.remove(plate);

    undoStack.removePlate(plate);

    if (loc.find("VIP") != string::npos) {
        int idx = stoi(loc.substr(loc.find_last_of(' ') + 1)) - 1;
        slots.freeVIP(idx);
    } else if (loc.find("Regular") != string::npos) {
        int idx = stoi(loc.substr(loc.find_last_of(' ') + 1)) - 1;
        slots.freeRegular(idx);
    }

    if (!waitingQueue.empty()) {
        string nextPlate = waitingQueue.front();
        waitingQueue.pop();

        if (loc.find("VIP") != string::npos) {
            int idx = stoi(loc.substr(loc.find_last_of(' ') + 1)) - 1;
            slots.occupyVIP(idx, nextPlate);
            vehicleMap.insert(nextPlate, "VIP Slot " + to_string(idx + 1));
        } else if (loc.find("Regular") != string::npos) {
            int idx = stoi(loc.substr(loc.find_last_of(' ') + 1)) - 1;
            slots.occupyRegular(idx, nextPlate);
            vehicleMap.insert(nextPlate, "Regular Slot " + to_string(idx + 1));
        }
    }

    return "{\"status\":\"success\",\"message\":\"exited successfully!\"}";
}