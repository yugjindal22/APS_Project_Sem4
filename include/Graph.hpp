#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <set>

using namespace std;

class Graph
{
private:
    unordered_map<string, vector<string>> adjacencyList;
    vector<vector<int>> adjacencyMatrix;
    unordered_map<string, int> userIndices;
    vector<string> users;

    // Network flow related data structures
    vector<vector<int>> capacityMatrix; // For Ford-Fulkerson
    vector<vector<int>> flowMatrix;     // For tracking message flow

    // Helper function for network flow visualization
    string getFlowVisualization(int source, int sink, const vector<vector<int>> &flow) const;
    void printAugmentingPath(const vector<int> &parent, int source, int sink) const;

public:
    Graph();

    // Basic graph operations
    void addUser(const string &userId);
    void addConnection(const string &user1, const string &user2);
    void removeConnection(const string &user1, const string &user2);
    bool areConnected(const string &user1, const string &user2) const;

    // Friend recommendations using BFS
    vector<string> getFriendRecommendations(const string &userId, int depth = 2) const;

    // BFS traversal
    vector<string> BFS(const string &startUser) const;

    // DFS traversal
    vector<string> DFS(const string &startUser) const;
    void DFSUtil(const string &user, set<string> &visited, vector<string> &result) const;

    // Community detection using Kruskal's algorithm
    struct Edge
    {
        string user1;
        string user2;
        int weight;

        Edge(string u1, string u2, int w) : user1(u1), user2(u2), weight(w) {}
    };

    vector<vector<string>> detectCommunities(int threshold);

    // Floyd-Warshall algorithm implementation
    vector<vector<int>> floydWarshall() const;

    // Network Flow (Ford-Fulkerson) for message routing
    bool sendMessage(const string &fromUser, const string &toUser, const string &message);
    int maxFlow(int source, int sink);                   // Ford-Fulkerson implementation
    bool bfs(int source, int sink, vector<int> &parent); // For Ford-Fulkerson

    // Dynamic Programming for message path optimization
    vector<string> findOptimalMessagePath(const string &fromUser, const string &toUser);

    // Utility functions
    const unordered_map<string, vector<string>> &getAdjacencyList() const;
    const vector<string> &getUsers() const;
    int getUserCount() const;

    // Network flow visualization accessors
    const vector<vector<int>> &getCapacityMatrix() const { return capacityMatrix; }
    const vector<vector<int>> &getFlowMatrix() const { return flowMatrix; }

private:
    // Helper functions for community detection
    vector<Edge> getAllEdges() const;
    int find(vector<int> &parent, int i);
    void unionSets(vector<int> &parent, vector<int> &rank, int x, int y);

    void initializeFlowNetworks(); // Initialize flow networks
};

#endif // GRAPH_HPP