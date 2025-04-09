#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <set>

class Graph
{
private:
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;
    std::vector<std::vector<int>> adjacencyMatrix;
    std::unordered_map<std::string, int> userIndices;
    std::vector<std::string> users;

public:
    Graph();

    // Basic graph operations
    void addUser(const std::string &userId);
    void addConnection(const std::string &user1, const std::string &user2);
    void removeConnection(const std::string &user1, const std::string &user2);
    bool areConnected(const std::string &user1, const std::string &user2) const;

    // Friend recommendations using BFS
    std::vector<std::string> getFriendRecommendations(const std::string &userId, int depth = 2) const;

    // BFS traversal
    std::vector<std::string> BFS(const std::string &startUser) const;

    // DFS traversal
    std::vector<std::string> DFS(const std::string &startUser) const;
    void DFSUtil(const std::string &user, std::set<std::string> &visited, std::vector<std::string> &result) const;

    // Community detection using Kruskal's algorithm
    struct Edge
    {
        std::string user1;
        std::string user2;
        int weight;

        Edge(std::string u1, std::string u2, int w) : user1(u1), user2(u2), weight(w) {}
    };

    std::vector<std::vector<std::string>> detectCommunities(int threshold);

    // Floyd-Warshall algorithm implementation
    std::vector<std::vector<int>> floydWarshall() const;

    // Utility functions
    const std::unordered_map<std::string, std::vector<std::string>> &getAdjacencyList() const;
    const std::vector<std::string> &getUsers() const;
    int getUserCount() const;

private:
    // Helper functions for community detection
    std::vector<Edge> getAllEdges() const;
    int find(std::vector<int> &parent, int i);
    void unionSets(std::vector<int> &parent, std::vector<int> &rank, int x, int y);
};

#endif // GRAPH_HPP