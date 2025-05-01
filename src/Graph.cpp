#include "../include/Graph.hpp"
#include <algorithm>
#include <limits>
#include <stack>
#include <iostream> // Added for cout
#include <queue>
#include <unordered_map>
#include <set>
#include <vector>

using namespace std;

Graph::Graph() {}

void Graph::addUser(const string &userId)
{
    if (adjacencyList.find(userId) == adjacencyList.end())
    {
        adjacencyList[userId] = vector<string>();

        // Update users and indices
        userIndices[userId] = users.size();
        users.push_back(userId);

        // Update adjacency matrix
        size_t newSize = users.size();
        adjacencyMatrix.resize(newSize);
        for (auto &row : adjacencyMatrix)
        {
            row.resize(newSize, numeric_limits<int>::max());
        }
        // Set diagonal to 0
        adjacencyMatrix[newSize - 1][newSize - 1] = 0;

        initializeFlowNetworks();
    }
}

void Graph::addConnection(const string &user1, const string &user2)
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

        initializeFlowNetworks();
    }
}

void Graph::removeConnection(const string &user1, const string &user2)
{
    if (adjacencyList.find(user1) != adjacencyList.end() && adjacencyList.find(user2) != adjacencyList.end())
    {
        adjacencyList[user1].erase(
            remove(adjacencyList[user1].begin(), adjacencyList[user1].end(), user2),
            adjacencyList[user1].end());
        adjacencyList[user2].erase(
            remove(adjacencyList[user2].begin(), adjacencyList[user2].end(), user1),
            adjacencyList[user2].end());

        // Update adjacency matrix
        int idx1 = userIndices[user1];
        int idx2 = userIndices[user2];
        adjacencyMatrix[idx1][idx2] = numeric_limits<int>::max();
        adjacencyMatrix[idx2][idx1] = numeric_limits<int>::max();
    }
}

bool Graph::areConnected(const string &user1, const string &user2) const
{
    if (adjacencyList.find(user1) == adjacencyList.end() || adjacencyList.find(user2) == adjacencyList.end())
    {
        return false;
    }
    return std::find(adjacencyList.at(user1).begin(), adjacencyList.at(user1).end(), user2) != adjacencyList.at(user1).end();
}

// Breadth First Search (BFS) algorithm implementation - O(V + E)
// Used for finding friends at different depths in the social network
vector<string> Graph::getFriendRecommendations(const string &userId, int depth) const
{
    vector<string> recommendations;
    set<string> visited;
    queue<pair<string, int>> queue;

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

// Breadth First Search (BFS) algorithm implementation - O(V + E)
// Used for traversing the social network level by level
vector<string> Graph::BFS(const string &startUser) const
{
    vector<string> result;
    set<string> visited;
    queue<string> queue;

    visited.insert(startUser);
    queue.push(startUser);

    while (!queue.empty())
    {
        string current = queue.front();
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

// Depth First Search (DFS) algorithm implementation - O(V + E)
// Used for exploring social network paths as deeply as possible
vector<string> Graph::DFS(const string &startUser) const
{
    vector<string> result;
    set<string> visited;
    DFSUtil(startUser, visited, result);
    return result;
}

void Graph::DFSUtil(const string &user, set<string> &visited, vector<string> &result) const
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

vector<Graph::Edge> Graph::getAllEdges() const
{
    vector<Edge> edges;
    set<pair<string, string>> added;

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

int Graph::find(vector<int> &parent, int i)
{
    if (parent[i] != i)
    {
        parent[i] = find(parent, parent[i]);
    }
    return parent[i];
}

void Graph::unionSets(vector<int> &parent, vector<int> &rank, int x, int y)
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

// Kruskal's Minimum Spanning Tree algorithm with Union-Find data structure
// Time Complexity: O(E log E) where E is number of edges
// Used for community detection by treating edge weights as connection strengths
vector<vector<string>> Graph::detectCommunities(int threshold)
{
    vector<Edge> edges = getAllEdges();
    sort(edges.begin(), edges.end(),
         [](const Edge &a, const Edge &b)
         { return a.weight < b.weight; });

    vector<int> parent(users.size());
    vector<int> rank(users.size(), 0);
    for (size_t i = 0; i < users.size(); i++)
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

    unordered_map<int, vector<string>> communities;
    for (size_t i = 0; i < users.size(); i++)
    {
        int root = find(parent, i);
        communities[root].push_back(users[i]);
    }

    vector<vector<string>> result;
    for (const auto &community : communities)
    {
        result.push_back(community.second);
    }

    return result;
}

// Floyd-Warshall Algorithm for All-Pairs Shortest Paths
// Time Complexity: O(V^3) where V is number of vertices
// Used to find shortest paths (degrees of separation) between all pairs of users
vector<vector<int>> Graph::floydWarshall() const
{
    int V = users.size();
    auto dist = adjacencyMatrix;

    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (dist[i][k] != numeric_limits<int>::max() &&
                    dist[k][j] != numeric_limits<int>::max() &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    return dist;
}

const unordered_map<string, vector<string>> &Graph::getAdjacencyList() const
{
    return adjacencyList;
}

const vector<string> &Graph::getUsers() const
{
    return users;
}

int Graph::getUserCount() const
{
    return users.size();
}

// Initialize flow networks when adding users or connections
void Graph::initializeFlowNetworks()
{
    int n = users.size();
    // Reset matrices with proper sizes
    capacityMatrix = vector<vector<int>>(n, vector<int>(n, 0));
    flowMatrix = vector<vector<int>>(n, vector<int>(n, 0));

    // Set capacity 1 for ALL connections in the adjacency list
    for (const auto &[user, friends] : adjacencyList)
    {
        int u = userIndices[user];
        for (const auto &friend_ : friends)
        {
            int v = userIndices[friend_];
            // Set both directions since the network is undirected
            capacityMatrix[u][v] = 1;
            capacityMatrix[v][u] = 1;
        }
    }
}

// Ford-Fulkerson implementation using BFS for finding augmenting paths
bool Graph::bfs(int source, int sink, vector<int> &parent)
{
    int V = users.size();
    vector<bool> visited(V, false);
    queue<int> q;

    q.push(source);
    visited[source] = true;
    parent[source] = -1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; v++)
        {
            if (!visited[v] && capacityMatrix[u][v] > flowMatrix[u][v])
            {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }

    return visited[sink];
}

// Helper function to visualize network flow
string Graph::getFlowVisualization(int source, int sink, const vector<vector<int>> &flow) const
{
    string result = "\nNetwork Flow Visualization:\n";
    result += "Each line shows: user -> user (flow/capacity)\n\n";

    for (size_t i = 0; i < users.size(); i++)
    {
        for (size_t j = 0; j < users.size(); j++)
        {
            if (capacityMatrix[i][j] > 0)
            {
                string arrow = (static_cast<int>(i) == source || static_cast<int>(j) == sink) ? "=>" : "->";
                result += users[i] + " " + arrow + " " + users[j] +
                          " (" + to_string(flow[i][j]) + "/" +
                          to_string(capacityMatrix[i][j]) + ")\n";
            }
        }
    }
    return result;
}

void Graph::printAugmentingPath(const vector<int> &parent, int source, int sink) const
{
    cout << "Found augmenting path: ";
    vector<int> path;
    for (int v = sink; v != source; v = parent[v])
    {
        path.push_back(v);
    }
    path.push_back(source);

    for (int i = path.size() - 1; i >= 0; i--)
    {
        cout << users[path[i]];
        if (i > 0)
            cout << " -> ";
    }
    cout << "\n";
}

// Update the Ford-Fulkerson maxFlow implementation to show visualization
int Graph::maxFlow(int source, int sink)
{
    vector<int> parent(users.size());
    int maxFlow = 0;

    cout << "\nStarting Ford-Fulkerson algorithm for message routing...\n";
    cout << getFlowVisualization(source, sink, flowMatrix);

    // While there is an augmenting path from source to sink
    int pathCount = 0;
    while (bfs(source, sink, parent))
    {
        pathCount++;
        cout << "\nStep " << pathCount << ":\n";
        printAugmentingPath(parent, source, sink);

        int pathFlow = numeric_limits<int>::max();

        // Find minimum residual capacity along the path
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];
            pathFlow = min(pathFlow, capacityMatrix[u][v] - flowMatrix[u][v]);
        }

        cout << "Path capacity: " << pathFlow << "\n";

        // Update residual capacities and reverse edges
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];
            flowMatrix[u][v] += pathFlow;
            flowMatrix[v][u] -= pathFlow;
        }

        maxFlow += pathFlow;
        cout << getFlowVisualization(source, sink, flowMatrix);
    }

    cout << "\nFinal maximum flow: " << maxFlow << "\n";
    return maxFlow;
}

// Dynamic Programming for finding optimal message path
// Uses Floyd-Warshall results to find path with minimum hops
vector<string> Graph::findOptimalMessagePath(const string &fromUser, const string &toUser)
{
    vector<string> path;
    if (adjacencyList.find(fromUser) == adjacencyList.end() ||
        adjacencyList.find(toUser) == adjacencyList.end())
    {
        return path;
    }

    int start = userIndices[fromUser];
    int end = userIndices[toUser];

    // Get shortest paths using Floyd-Warshall
    auto distances = floydWarshall();

    // Use DP to reconstruct the path
    vector<vector<int>> next(users.size(), vector<int>(users.size(), -1));

    // Initialize next matrix
    for (size_t i = 0; i < users.size(); i++)
    {
        for (size_t j = 0; j < users.size(); j++)
        {
            if (distances[i][j] != numeric_limits<int>::max())
            {
                next[i][j] = j;
            }
        }
    }

    // Find path using next matrix
    if (distances[start][end] == numeric_limits<int>::max())
    {
        return path; // No path exists
    }

    // Reconstruct path
    int current = start;
    path.push_back(users[current]);
    while (current != end)
    {
        current = next[current][end];
        path.push_back(users[current]);
    }

    return path;
}

// Update sendMessage to show the visualization
bool Graph::sendMessage(const string &fromUser, const string &toUser, const string &message [[maybe_unused]])
{
    if (adjacencyList.find(fromUser) == adjacencyList.end() ||
        adjacencyList.find(toUser) == adjacencyList.end())
    {
        return false;
    }

    cout << "\nAttempting to send message from " << fromUser << " to " << toUser << "...\n";

    initializeFlowNetworks();
    int source = userIndices[fromUser];
    int sink = userIndices[toUser];

    cout << "\nInitial network state:\n";
    cout << getFlowVisualization(source, sink, flowMatrix);

    // Check if message can be routed (max flow > 0)
    int flow = maxFlow(source, sink);

    if (flow > 0)
    {
        cout << "\nMessage can be routed! Maximum possible simultaneous messages: " << flow << "\n";
        return true;
    }
    else
    {
        cout << "\nNo valid path exists for the message.\n";
        return false;
    }
}