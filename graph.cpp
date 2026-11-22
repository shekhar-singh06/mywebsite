#include "graph.h"
#include <sstream>

void Graph::addEdge(string u, string v, int weight) {
    adjList[u].push_back({v, weight});
    adjList[v].push_back({u, weight}); // Undirected graph (two-way road)
}

string Graph::findShortestPath(string startNode, string endNode) {
    unordered_map<string, int> distances;
    unordered_map<string, string> parent;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

    for (auto const& pair : adjList) {
        distances[pair.first] = INT_MAX;
    }

    distances[startNode] = 0;
    pq.push({0, startNode});

    while (!pq.empty()) {
        int dist = pq.top().first;
        string u = pq.top().second;
        pq.pop();

        if (u == endNode) break;

        if (dist > distances[u]) continue;

        for (auto& neighbor : adjList[u]) {
            string v = neighbor.first;
            int weight = neighbor.second;

            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                parent[v] = u;
                pq.push({distances[v], v});
            }
        }
    }

    // Reconstruct path as JSON array for frontend
    if (distances[endNode] == INT_MAX) {
        return "{\"status\":\"error\",\"message\":\"Path not found\"}";
    }

    vector<string> path;
    string curr = endNode;
    while (curr != startNode) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(startNode);

    // Reverse to get path from start to end
    stringstream ss;
    ss << "{\"status\":\"success\",\"distance\":" << distances[endNode] << ",\"path\":[";
    for (int i = path.size() - 1; i >= 0; --i) {
        ss << "\"" << path[i] << "\"";
        if (i > 0) ss << ",";
    }
    ss << "]}";

    return ss.str();
}