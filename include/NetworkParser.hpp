#ifndef NETWORK_PARSER_HPP
#define NETWORK_PARSER_HPP

#include <string>
#include <vector>
#include "User.hpp"
#include "Graph.hpp"

using namespace std;

class NetworkParser
{
public:
    // JSON parsing
    static bool parseJSONFile(const string &filename, Graph &graph, vector<User *> &users);
    static bool parseCSVFile(const string &filename, Graph &graph, vector<User *> &users);

    // Data export
    static bool exportToJSON(const string &filename, const Graph &graph, const vector<User *> &users);
    static bool exportToCSV(const string &filename, const Graph &graph, const vector<User *> &users);

private:
    // Helper functions for JSON parsing
    static string readFile(const string &filename);
    static void parseUserData(const string &jsonStr, size_t &pos, User *&user);
    static void parseConnections(const string &jsonStr, size_t &pos, Graph &graph);
    static string parseString(const string &jsonStr, size_t &pos);
    static int parseNumber(const string &jsonStr, size_t &pos);
    static void skipWhitespace(const string &jsonStr, size_t &pos);
};

#endif // NETWORK_PARSER_HPP