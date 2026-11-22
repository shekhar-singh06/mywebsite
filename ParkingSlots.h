#ifndef PARKINGSLOTS_H
#define PARKINGSLOTS_H

#include <vector>
#include <string>
using namespace std;

class ParkingSlots
{
private:
    vector<string> regularSlots;
    vector<string> vipSlots;
    string vectorToJson(const vector<string>& vec);

public:
    ParkingSlots();

    int getFreeRegularSlot();
    int getFreeVIPSlot();

    void occupyRegular(int index, string plate);
    void occupyVIP(int index, string plate);

    void freeRegular(int index);
    void freeVIP(int index);

    vector<string> getRegularSlots();
    vector<string> getVIPSlots();

    string getRegularJson();
    string getVIPJson();
};

#endif