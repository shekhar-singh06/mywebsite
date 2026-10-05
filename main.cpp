#include "httplib.h"
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <string>
using namespace std;
struct ActionLog {
    string plate;
    string type; 
    int index;   
};
vector<string> regularSlots(10, "FREE");
vector<string> vipSlots(5, "FREE");
queue<string> waitingQueue;
unordered_map<string, string> vehicleHashMap;
stack<ActionLog> undoStack; 
string vectorToJson(const vector<string>& vec) {
    string json = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        json += "\"" + vec[i] + "\"" + (i < vec.size() - 1 ? "," : "");
    }
    return json + "]";
}
string queueToJson(queue<string> q) {
    string json = "[";
    while (!q.empty()) {
        json += "\"" + q.front() + "\"";
        q.pop();
        if (!q.empty()) json += ",";
    }
    return json + "]";
}

int main() {
    httplib::Server svr;
    svr.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        string json = "{";
        json += "\"regular\":" + vectorToJson(regularSlots) + ",";
        json += "\"vip\":" + vectorToJson(vipSlots) + ",";
        json += "\"queue\":" + queueToJson(waitingQueue) + ",";
        json += "\"stackSize\":" + to_string(undoStack.size());
        json += "}";
        res.set_content(json, "application/json");
    });
    svr.Post("/api/entry", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        string plate = req.get_param_value("plate");
        string type = req.get_param_value("type");
        if (plate.empty()) {
            res.set_content("{\"status\":\"error\", \"message\":\"License Plate is required!\"}", "application/json");
            return;
        }
        string allocated = "";
        ActionLog action;
        action.plate = plate;
        if (type == "VIP") {
            for (size_t i = 0; i < vipSlots.size(); ++i) {
                if (vipSlots[i] == "FREE") {
                    vipSlots[i] = plate;
                    allocated = "VIP Slot " + to_string(i + 1);
                    action.type = "VIP";
                    action.index = i;
                    break;
                }
            }
        } 
        if (allocated == "") {
            for (size_t i = 0; i < regularSlots.size(); ++i) {
                if (regularSlots[i] == "FREE") {
                    regularSlots[i] = plate;
                    allocated = "Regular Slot " + to_string(i + 1);
                    action.type = "REGULAR";
                    action.index = i;
                    break;
                }
            }
        }
        if (allocated == "") {
            waitingQueue.push(plate);
            allocated = "Waiting Queue (FIFO)";
            action.type = "QUEUE";
            action.index = -1;
        }

        vehicleHashMap[plate] = allocated;
        undoStack.push(action);
        cout << "[C++ STACK PUSH] Vehicle Parked: " << plate << " | Stack Size: " << undoStack.size() << endl;

        res.set_content("{\"status\":\"success\", \"location\":\"" + allocated + "\"}", "application/json");
    });
    svr.Post("/api/undo", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        
        if (undoStack.empty()) {
            res.set_content("{\"status\":\"error\", \"message\":\"Stack is Empty! No entry to undo.\"}", "application/json");
            return;
        }
        ActionLog lastAction = undoStack.top();
        undoStack.pop();
        string msg = "";
        if (lastAction.type == "REGULAR") {
            regularSlots[lastAction.index] = "FREE";
            msg = "Undone Regular Slot " + to_string(lastAction.index + 1) + " for " + lastAction.plate;
        } else if (lastAction.type == "VIP") {
            vipSlots[lastAction.index] = "FREE";
            msg = "Undone VIP Slot " + to_string(lastAction.index + 1) + " for " + lastAction.plate;
        } else if (lastAction.type == "QUEUE") {
            queue<string> tempQ;
            while (!waitingQueue.empty()) {
                if (waitingQueue.front() != lastAction.plate) {
                    tempQ.push(waitingQueue.front());
                }
                waitingQueue.pop();
            }
            waitingQueue = tempQ;
            msg = "Removed " + lastAction.plate + " from Waiting Queue";
        }
        vehicleHashMap.erase(lastAction.plate);
        cout << "[C++ STACK POP] Undone Entry: " << lastAction.plate << endl;
        res.set_content("{\"status\":\"success\", \"message\":\"" + msg + "\"}", "application/json");
    });
    svr.Post("/api/exit", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        string plate = req.get_param_value("plate");
        bool found = false;
        string msg = "";
        for (size_t i = 0; i < regularSlots.size(); ++i) {
            if (regularSlots[i] == plate) {
                regularSlots[i] = "FREE";
                found = true;
                msg = "Exited from Regular Slot " + to_string(i + 1);
                if (!waitingQueue.empty()) {
                    string nextCar = waitingQueue.front();
                    waitingQueue.pop();
                    regularSlots[i] = nextCar;
                    vehicleHashMap[nextCar] = "Regular Slot " + to_string(i + 1);
                }
                break;
            }
        }
        if (!found) {
            for (size_t i = 0; i < vipSlots.size(); ++i) {
                if (vipSlots[i] == plate) {
                    vipSlots[i] = "FREE";
                    found = true;
                    msg = "Exited from VIP Slot " + to_string(i + 1);
                    break;
                }
            }
        }
        if (found) {
            vehicleHashMap.erase(plate);
            res.set_content("{\"status\":\"success\", \"message\":\"" + msg + "\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"error\", \"message\":\"Vehicle not found!\"}", "application/json");
        }
    });
    svr.Get("/api/search", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        string plate = req.get_param_value("plate");
        if (vehicleHashMap.find(plate) != vehicleHashMap.end()) {
            res.set_content("{\"found\":true, \"location\":\"" + vehicleHashMap[plate] + "\"}", "application/json");
        } else {
            res.set_content("{\"found\":false}", "application/json");
        }
    });
    char* portStr = getenv("PORT");
    int port = portStr ? atoi(portStr) : 8080;

    cout << "========================================================\n";
    cout << "   PARKFLOW: C++ STACK & DSA SERVER RUNNING PORT " << port << "\n";
    cout << "========================================================\n";
    svr.set_mount_point("/", ".");
    svr.listen("0.0.0.0", port);
    return 0;
}
