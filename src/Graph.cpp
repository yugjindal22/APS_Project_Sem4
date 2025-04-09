#include "../include/Graph.hpp"
#include <algorithm>
#include <limits>
#include <stack>

Graph::Graph() {}

void Graph::addUser(const std::string &userId)
{
    if (adjacencyList.find(userId) == adjacencyList.end())
    {
        adjacencyList[userId] = std::vector<std::string>();

        // Update users and indices
        userIndices[userId] = users.size();
        users.push_back(userId);

        // Update adjacency matrix
        size_t newSize = users.size();
        adjacencyMatrix.resize(newSize);
        for (auto &row : adjacencyMatrix)
        {
            row.resize(newSize, std::numeric_limits<int>::max());
        }
        // Set diagonal to 0
        adjacencyMatrix[newSize - 1][newSize - 1] = 0;
    }
}

void Graph::addConnection(const std::string &user1, const std::string &user2)
{
    if (user1 == user2)
        return;

    // Make sure both users exist
    addUser(user1);
    addUser(user2);

    // Update adjacency list
    if (std::find(adjacencyList[user1].begin(), adjacencyList[user1].end(), user2) == adjacencyList[user1].end())
    {
        adjacencyList[user1].push_back(user2);
        adjacencyList[user2].push_back(user1);

        // Update adjacency matrix
        int idx1 = userIndices[user1];
        int idx2 = userIndices[user2];
        adjacencyMatrix[idx1][idx2] = 1;
        adjacencyMatrix[idx2][idx1] = 1;
    }
}

void Graph::removeConnection(const std::string &user1, const std::string &user2)
{
    if (adjacencyList.find(user1) != adjacencyList.end() && adjacencyList.find(user2) != adjacencyList.end())
    {
        adjacencyList[user1].erase(
            std::remove(adjacencyList[user1].begin(), adjacencyList[user1].end(), user2),
            adjacencyList[user1].end());
        adjacencyList[user2].erase(
            std::remove(adjacencyList[user2].begin(), adjacencyList[user2].end(), user1),
            adjacencyList[user2].end());

        // Update adjacency matrix
        int idx1 = userIndices[user1];
        int idx2 = userIndices[user2];
        adjacencyMatrix[idx1][idx2] = std::numeric_limits<int>::max();
        adjacencyMatrix[idx2][idx1] = std::numeric_limits<int>::max();
    }
}

bool Graph::areConnected(const std::string &user1, const std::string &user2) const
{
    if (adjacencyList.find(user1) == adjacencyList.end() || adjacencyList.find(user2) == adjacencyList.end())
    {
        return false;
    }
    return std::find(adjacencyList.at(user1).begin(), adjacencyList.at(user1).end(), user2) != adjacencyList.at(user1).end();
}

std::vector<std::string> Graph::getFriendRecommendations(const std::string &userId, int depth) const
{
    std::vector<std::string> recommendations;
    std::set<std::string> visited;
    std::queue<std::pair<std::string, int>> queue;

    visited.insert(userId);
    queue.push({userId, 0});

    while (!queue.empty())
    {
        auto [currentUser, currentDepth] = queue.front();
        queue.pop();

        if (currentDepth >= depth)
            continue;

        for (const auto &friend_ : adjacencyList.at(currentUser))
        {
            if (visited.find(friend_) == visited.end())
            {
                visited.insert(friend_);
                queue.push({friend_, currentDepth + 1});
                if (currentDepth > 0)
                { // Don't include direct friends
                    recommendations.push_back(friend_);
                }
            }
        }
    }

    return recommendations;
}

std::vector<std::string> Graph::BFS(const std::string &startUser) const
{
    std::vector<std::string> result;
    std::set<std::string> visited;
    std::queue<std::string> queue;

    visited.insert(startUser);
    queue.push(startUser);

    while (!queue.empty())
    {
        std::string current = queue.front();
        queue.pop();
        result.push_back(current);

        for (const auto &neighbor : adjacencyList.at(current))
        {
            if (visited.find(neighbor) == visited.end())
            {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }

    return result;
}

std::vector<std::string> Graph::DFS(const std::string &startUser) const
{
    std::vector<std::string> result;
    std::set<std::string> visited;
    DFSUtil(startUser, visited, result);
    return result;
}

void Graph::DFSUtil(const std::string &user, std::set<std::string> &visited, std::vector<std::string> &result) const
{
    visited.insert(user);
    result.push_back(user);

    for (const auto &neighbor : adjacencyList.at(user))
    {
        if (visited.find(neighbor) == visited.end())
        {
            DFSUtil(neighbor, visited, result);
        }
    }
}

std::vector<Graph::Edge> Graph::getAllEdges() const
{
    std::vector<Edge> edges;
    std::set<std::pair<std::string, std::string>> added;

    for (const auto &[user, friends] : adjacencyList)
    {
        for (const auto &friend_ : friends)
        {
            if (user < friend_ && added.find({user, friend_}) == added.end())
            {
                edges.emplace_back(user, friend_, 1); // Using weight 1 for simplicity
                added.insert({user, friend_});
            }
        }
    }

    return edges;
}

int Graph::find(std::vector<int> &parent, int i)
{
    if (parent[i] != i)
    {
        parent[i] = find(parent, parent[i]);
    }
    return parent[i];
}

void Graph::unionSets(std::vector<int> &parent, std::vector<int> &rank, int x, int y)
{
    int rootX = find(parent, x);
    int rootY = find(parent, y);

    if (rank[rootX] < rank[rootY])
    {
        parent[rootX] = rootY;
    }
    else if (rank[rootX] > rank[rootY])
    {
        parent[rootY] = rootX;
    }
    else
    {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

std::vector<std::vector<std::string>> Graph::detectCommunities(int threshold)
{
    std::vector<Edge> edges = getAllEdges();
    std::sort(edges.begin(), edges.end(),
              [](const Edge &a, const Edge &b)
              { return a.weight < b.weight; });

    std::vector<int> parent(users.size());
    std::vector<int> rank(users.size(), 0);
    for (int i = 0; i < users.size(); i++)
    {
        parent[i] = i;
    }

    for (const auto &edge : edges)
    {
        int u = userIndices[edge.user1];
        int v = userIndices[edge.user2];

        if (find(parent, u) != find(parent, v))
        {
            if (edge.weight <= threshold)
            {
                unionSets(parent, rank, u, v);
            }
        }
    }

    std::unordered_map<int, std::vector<std::string>> communities;
    for (int i = 0; i < users.size(); i++)
    {
        int root = find(parent, i);
        communities[root].push_back(users[i]);
    }

    std::vector<std::vector<std::string>> result;
    for (const auto &community : communities)
    {
        result.push_back(community.second);
    }

    return result;
}

std::vector<std::vector<int>> Graph::floydWarshall() const
{
    int V = users.size();
    auto dist = adjacencyMatrix;

    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (dist[i][k] != std::numeric_limits<int>::max() &&
                    dist[k][j] != std::numeric_limits<int>::max() &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    return dist;
}

const std::unordered_map<std::string, std::vector<std::string>> &Graph::getAdjacencyList() const
{
    return adjacencyList;
}

const std::vector<std::string> &Graph::getUsers() const
{
    return users;
}

int Graph::getUserCount() const
{
    return users.size();
}