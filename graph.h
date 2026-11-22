#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <climits>

using namespace std;

class Graph {
private:
    // Adjacency list: Node -> list of (Neighbor, weight/distance)
    unordered_map<string, vector<pair<string, int>>> adjList;

public:
    void addEdge(string u, string v, int weight);
    string findShortestPath(string startNode, string endNode);
};

#endif