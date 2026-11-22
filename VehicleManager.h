#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <string>
#include "ParkingSlots.h"
#include "waitingqueue.h"
#include "VehicleHashMap.h"
#include "UndoStack.h"

using namespace std;

class VehicleManager
{
public:
    static string handleEntry(string plate, string type, ParkingSlots& slots, waitingqueue& waitingQueue, VehicleHashMap& vehicleMap, UndoStack& undoStack);
    static string handleExit(string plate, ParkingSlots& slots, waitingqueue& waitingQueue, VehicleHashMap& vehicleMap, UndoStack& undoStack);
};

#endif