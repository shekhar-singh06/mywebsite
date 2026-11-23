#include <iostream>
#include <cstdlib>
#include <string>
#include "httplib.h"
#include "ParkingLot.h"
#include "graph.h"

#include "ParkingLot.cpp"
#include "waitingqueue.cpp"
#include "VehicleManager.cpp"
#include "ParkingSlots.cpp"
#include "UndoStack.cpp"
#include "VehicleHashMap.cpp"
#include "graph.cpp"

using namespace std;

int main() {
    ParkingLot parkingLot;
    
    // Graph aur intersections initialization
    Graph parkingGraph;
    parkingGraph.addEdge("Gate", "Intersection1", 5);
    parkingGraph.addEdge("Intersection1", "Intersection_Reg_1", 6);
    parkingGraph.addEdge("Intersection_Reg_1", "Intersection_Reg_2", 7);

    // 5 VIP Slots ke liye edges (VIP_Slot_1 to VIP_Slot_5)
    for (int i = 1; i <= 5; ++i) {
        string slotName = "VIP_Slot_" + to_string(i);
        parkingGraph.addEdge("Intersection1", slotName, 5 + (i * 5));
    }

    // First Regular Intersection (Slot_1 to Slot_5)
    for (int i = 1; i <= 5; ++i) {
        string slotName = "Slot_" + to_string(i);
        parkingGraph.addEdge("Intersection_Reg_1", slotName, 4 + (i * 5));
    }

    // Second Regular Intersection (Slot_6 to Slot_10)
    for (int i = 6; i <= 10; ++i) {
        string slotName = "Slot_" + to_string(i);
        parkingGraph.addEdge("Intersection_Reg_2", slotName, 4 + ((i - 5) * 5));
    }

    httplib::Server svr;

    auto set_cors_headers = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    svr.Options(R"(/.*)", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        res.status = 200;
    });

    svr.Get("/api/status", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string jsonStatus = parkingLot.getStatus();
        res.set_content(jsonStatus, "application/json");
    });

    svr.Post("/api/entry", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string plate = req.has_param("plate") ? req.get_param_value("plate") : "";
        string type = req.has_param("type") ? req.get_param_value("type") : "REGULAR";
        
        string responseJson = parkingLot.vehicleEntry(plate, type);
        res.set_content(responseJson, "application/json");
    });

    svr.Post("/api/exit", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string plate = req.has_param("plate") ? req.get_param_value("plate") : "";
        
        string responseJson = parkingLot.vehicleExit(plate);
        res.set_content(responseJson, "application/json");
    });

    svr.Post("/api/undo", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string responseJson = parkingLot.undoLastAction();
        res.set_content(responseJson, "application/json");
    });

    svr.Get("/api/search", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string plate = req.has_param("plate") ? req.get_param_value("plate") : "";
        
        string responseJson = parkingLot.searchVehicle(plate);
        res.set_content(responseJson, "application/json");
    });

    // Dijkstra Shortest Path Navigation Endpoint
    svr.Get("/api/navigate", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors_headers(res);
        string destination = req.has_param("to") ? req.get_param_value("to") : "Slot_1";
        
        string responseJson = parkingGraph.findShortestPath("Gate", destination);
        res.set_content(responseJson, "application/json");
    });

    const char* port_env = getenv("PORT");
    int port = port_env ? stoi(port_env) : 8080;

    cout << "Parking Lot C++ Backend Engine Initialized Successfully!" << endl;
    cout << "Server running on port " << port << endl;

    svr.listen("0.0.0.0", port);

    return 0;
}
