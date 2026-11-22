#ifndef WAITINGQUEUE_H
#define WAITINGQUEUE_H

#include <queue>
#include <string>
using namespace std;

class waitingqueue
{
private:
    queue<string> q;
    string vectorToJson(const queue<string>& queueRef);

public:
    void push(string plate);
    void pop();
    string front();
    bool empty();
    queue<string> getQueue();
    string getQueueJson();
    void removePlate(string plate);
};

#endif