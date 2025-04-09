#ifndef NETWORK_PARSER_HPP
#define NETWORK_PARSER_HPP

#include <string>
#include <vector>
#include "User.hpp"
#include "Graph.hpp"

class NetworkParser
{
public:
    // JSON parsing
    static bool parseJSONFile(const std::string &filename, Graph &graph, std::vector<User *> &users);
    static bool parseCSVFile(const std::string &filename, Graph &graph, std::vector<User *> &users);

    // Data export
    static bool exportToJSON(const std::string &filename, const Graph &graph, const std::vector<User *> &users);
    static bool exportToCSV(const std::string &filename, const Graph &graph, const std::vector<User *> &users);

private:
    // Helper functions for JSON parsing
    static std::string readFile(const std::string &filename);
    static void parseUserData(const std::string &jsonStr, size_t &pos, User *&user);
    static void parseConnections(const std::string &jsonStr, size_t &pos, Graph &graph);
    static std::string parseString(const std::string &jsonStr, size_t &pos);
    static int parseNumber(const std::string &jsonStr, size_t &pos);
    static void skipWhitespace(const std::string &jsonStr, size_t &pos);
};

#endif // NETWORK_PARSER_HPP