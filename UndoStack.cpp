#include "UndoStack.h"

using namespace std;

ActionLog::ActionLog()
{
    plate = "";
    slotType = "";
    slotIndex = -1;
}

ActionLog::ActionLog(string p, string t, int idx)
{
    plate = p;
    slotType = t;
    slotIndex = idx;
}

string ActionLog::getPlate()
{
    return plate;
}

string ActionLog::getSlotType()
{
    return slotType;
}

int ActionLog::getSlotIndex()
{
    return slotIndex;
}

void UndoStack::push(const ActionLog& action)
{
    historyStack.push(action);
}

void UndoStack::pop()
{
    if (historyStack.empty())
    {
        return;
    }
    else
    {
        historyStack.pop();
    }
}

bool UndoStack::empty()
{
    if (historyStack.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t UndoStack::size()
{
    return historyStack.size();
}

ActionLog UndoStack::top()
{
    if (historyStack.empty())
    {
        return ActionLog();
    }
    else
    {
        return historyStack.top();
    }
}

void UndoStack::removePlate(string plate)
{
    stack<ActionLog> tempStack;

    while (!historyStack.empty())
    {
        ActionLog action = historyStack.top();
        historyStack.pop();

        if (action.getPlate() != plate)
        {
            tempStack.push(action);
        }
    }

    while (!tempStack.empty())
    {
        historyStack.push(tempStack.top());
        tempStack.pop();
    }
}

string UndoStack::undoLastAction(
    ParkingSlots& slots,
    waitingqueue& waitingQueue,
    VehicleHashMap& vehicleMap)
{
    if (historyStack.empty())
    {
        return "{\"status\":\"error\",\"message\":\"No actions available to undo!\"}";
    }

    ActionLog action = historyStack.top();
    historyStack.pop();

    string plate = action.getPlate();
    string type = action.getSlotType();
    int index = action.getSlotIndex();

    if (type == "VIP")
    {
        slots.freeVIP(index);
        vehicleMap.remove(plate);
    }
    else if (type == "REGULAR")
    {
        slots.freeRegular(index);
        vehicleMap.remove(plate);
    }
    else if (type == "QUEUE")
    {
        waitingQueue.removePlate(plate);
        vehicleMap.remove(plate);
    }

    return "{\"status\":\"success\",\"message\":\"Undone last action for vehicle " + plate + "\"}";
}