#include "waitingqueue.h"

void waitingqueue::push(string plate)
{
    q.push(plate);
}

void waitingqueue::pop()
{
    if (!q.empty())
        q.pop();
}

string waitingqueue::front()
{
    if (q.empty())
        return "";
    return q.front();
}

bool waitingqueue::empty()
{
    return q.empty();
}

queue<string> waitingqueue::getQueue()
{
    return q;
}

string waitingqueue::vectorToJson(const queue<string>& queueRef)
{
    queue<string> tempQ = queueRef;
    string json = "[";
    while (!tempQ.empty())
    {
        json += "\"" + tempQ.front() + "\"";
        tempQ.pop();
        if (!tempQ.empty())
            json += ",";
    }
    json += "]";
    return json;
}

string waitingqueue::getQueueJson()
{
    return vectorToJson(q);
}

void waitingqueue::removePlate(string plate)
{
    queue<string> updatedQ;
    while (!q.empty()) {
        if (q.front() != plate) {
            updatedQ.push(q.front());
        }
        q.pop();
    }
    q = updatedQ;
}