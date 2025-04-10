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

    // Utility functions
    const unordered_map<string, vector<string>> &getAdjacencyList() const;
    const vector<string> &getUsers() const;
    int getUserCount() const;

private:
    // Helper functions for community detection
    vector<Edge> getAllEdges() const;
    int find(vector<int> &parent, int i);
    void unionSets(vector<int> &parent, vector<int> &rank, int x, int y);
};

#endif // GRAPH_HPP