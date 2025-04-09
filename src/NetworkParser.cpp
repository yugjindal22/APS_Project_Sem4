#include "../include/NetworkParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

std::string NetworkParser::readFile(const std::string &filename)
{
    std::cout << "Attempting to read file: " << filename << std::endl;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::cout << "Successfully read " << content.length() << " bytes" << std::endl;
    return content;
}

void NetworkParser::skipWhitespace(const std::string &jsonStr, size_t &pos)
{
    while (pos < jsonStr.length() && std::isspace(jsonStr[pos]))
    {
        pos++;
    }
}

std::string NetworkParser::parseString(const std::string &jsonStr, size_t &pos)
{
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length() || jsonStr[pos] != '"')
    {
        throw std::runtime_error("Expected string at position " + std::to_string(pos));
    }

    std::string result;
    pos++; // Skip opening quote

    while (pos < jsonStr.length() && jsonStr[pos] != '"')
    {
        if (jsonStr[pos] == '\\')
        {
            pos++;
            if (pos >= jsonStr.length())
            {
                throw std::runtime_error("Unterminated escape sequence");
            }
        }
        result += jsonStr[pos];
        pos++;
    }

    if (pos >= jsonStr.length())
    {
        throw std::runtime_error("Unterminated string");
    }

    pos++; // Skip closing quote
    return result;
}

int NetworkParser::parseNumber(const std::string &jsonStr, size_t &pos)
{
    skipWhitespace(jsonStr, pos);

    size_t start = pos;
    while (pos < jsonStr.length() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-'))
    {
        pos++;
    }

    return std::stoi(jsonStr.substr(start, pos - start));
}

void NetworkParser::parseUserData(const std::string &jsonStr, size_t &pos, User *&user)
{
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length() || jsonStr[pos] != '{')
    {
        throw std::runtime_error("Expected object at position " + std::to_string(pos));
    }

    pos++; // Skip {

    std::string id, name, location;
    int age = 0;
    std::vector<std::string> interests;

    while (pos < jsonStr.length() && jsonStr[pos] != '}')
    {
        skipWhitespace(jsonStr, pos);

        std::string key = parseString(jsonStr, pos);
        skipWhitespace(jsonStr, pos);

        if (pos >= jsonStr.length() || jsonStr[pos] != ':')
        {
            throw std::runtime_error("Expected ':' at position " + std::to_string(pos));
        }
        pos++; // Skip :

        if (key == "id")
        {
            id = parseString(jsonStr, pos);
        }
        else if (key == "name")
        {
            name = parseString(jsonStr, pos);
        }
        else if (key == "age")
        {
            age = parseNumber(jsonStr, pos);
        }
        else if (key == "location")
        {
            location = parseString(jsonStr, pos);
        }
        else if (key == "interests")
        {
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.length() || jsonStr[pos] != '[')
            {
                throw std::runtime_error("Expected array at position " + std::to_string(pos));
            }
            pos++; // Skip [

            while (pos < jsonStr.length() && jsonStr[pos] != ']')
            {
                interests.push_back(parseString(jsonStr, pos));
                skipWhitespace(jsonStr, pos);
                if (jsonStr[pos] == ',')
                    pos++;
            }

            if (pos >= jsonStr.length() || jsonStr[pos] != ']')
            {
                throw std::runtime_error("Unterminated array");
            }
            pos++; // Skip ]
        }

        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] == ',')
            pos++;
    }

    if (pos >= jsonStr.length() || jsonStr[pos] != '}')
    {
        throw std::runtime_error("Unterminated object");
    }
    pos++; // Skip }

    user = new User(id, name, age, location);
    for (const auto &interest : interests)
    {
        user->addInterest(interest);
    }
}

void NetworkParser::parseConnections(const std::string &jsonStr, size_t &pos, Graph &graph)
{
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length() || jsonStr[pos] != '[')
    {
        throw std::runtime_error("Expected array at position " + std::to_string(pos));
    }

    pos++; // Skip [

    while (pos < jsonStr.length() && jsonStr[pos] != ']')
    {
        skipWhitespace(jsonStr, pos);

        if (jsonStr[pos] != '{')
        {
            throw std::runtime_error("Expected object at position " + std::to_string(pos));
        }

        pos++; // Skip {

        std::string user1, user2;

        while (pos < jsonStr.length() && jsonStr[pos] != '}')
        {
            skipWhitespace(jsonStr, pos);

            std::string key = parseString(jsonStr, pos);
            skipWhitespace(jsonStr, pos);

            if (pos >= jsonStr.length() || jsonStr[pos] != ':')
            {
                throw std::runtime_error("Expected ':' at position " + std::to_string(pos));
            }
            pos++; // Skip :

            if (key == "user1")
            {
                user1 = parseString(jsonStr, pos);
            }
            else if (key == "user2")
            {
                user2 = parseString(jsonStr, pos);
            }

            skipWhitespace(jsonStr, pos);
            if (jsonStr[pos] == ',')
                pos++;
        }

        if (pos >= jsonStr.length() || jsonStr[pos] != '}')
        {
            throw std::runtime_error("Unterminated object");
        }
        pos++; // Skip }

        if (!user1.empty() && !user2.empty())
        {
            graph.addConnection(user1, user2);
        }

        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] == ',')
            pos++;
    }

    if (pos >= jsonStr.length() || jsonStr[pos] != ']')
    {
        throw std::runtime_error("Unterminated array");
    }
    pos++; // Skip ]
}

bool NetworkParser::parseJSONFile(const std::string &filename, Graph &graph, std::vector<User *> &users)
{
    try
    {
        std::string jsonStr = readFile(filename);
        size_t pos = 0;

        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.length() || jsonStr[pos] != '{')
        {
            throw std::runtime_error("Expected object at start of file");
        }
        pos++; // Skip {

        while (pos < jsonStr.length() && jsonStr[pos] != '}')
        {
            skipWhitespace(jsonStr, pos);

            std::string key = parseString(jsonStr, pos);
            skipWhitespace(jsonStr, pos);

            if (pos >= jsonStr.length() || jsonStr[pos] != ':')
            {
                throw std::runtime_error("Expected ':' at position " + std::to_string(pos));
            }
            pos++; // Skip :

            if (key == "users")
            {
                skipWhitespace(jsonStr, pos);
                if (jsonStr[pos] != '[')
                {
                    throw std::runtime_error("Expected array at position " + std::to_string(pos));
                }
                pos++; // Skip [

                while (pos < jsonStr.length() && jsonStr[pos] != ']')
                {
                    User *user = nullptr;
                    parseUserData(jsonStr, pos, user);
                    if (user)
                    {
                        users.push_back(user);
                        graph.addUser(user->getUserId());
                    }
                    skipWhitespace(jsonStr, pos);
                    if (jsonStr[pos] == ',')
                        pos++;
                }

                if (pos >= jsonStr.length() || jsonStr[pos] != ']')
                {
                    throw std::runtime_error("Unterminated array");
                }
                pos++; // Skip ]
            }
            else if (key == "connections")
            {
                parseConnections(jsonStr, pos, graph);
            }

            skipWhitespace(jsonStr, pos);
            if (jsonStr[pos] == ',')
                pos++;
        }

        if (pos >= jsonStr.length() || jsonStr[pos] != '}')
        {
            throw std::runtime_error("Unterminated object");
        }

        return true;
    }
    catch (const std::exception &e)
    {
        // Clean up any allocated users in case of error
        for (User *user : users)
        {
            delete user;
        }
        users.clear();
        return false;
    }
}

bool NetworkParser::parseCSVFile(const std::string &filename, Graph &graph, std::vector<User *> &users)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    try
    {
        std::string line;
        bool isFirstLine = true;
        std::vector<std::string> headers;

        while (std::getline(file, line))
        {
            if (isFirstLine)
            {
                std::stringstream ss(line);
                std::string header;
                while (std::getline(ss, header, ','))
                {
                    headers.push_back(header);
                }
                isFirstLine = false;
                continue;
            }

            std::stringstream ss(line);
            std::string value;
            std::vector<std::string> values;

            while (std::getline(ss, value, ','))
            {
                values.push_back(value);
            }

            if (values.size() >= 4)
            { // Assuming minimum required fields: id, name, age, location
                User *user = new User(values[0], values[1], std::stoi(values[2]), values[3]);
                users.push_back(user);
                graph.addUser(user->getUserId());

                // Add any additional fields as profile data
                for (size_t i = 4; i < values.size() && i < headers.size(); ++i)
                {
                    user->addProfileData(headers[i], values[i]);
                }
            }
        }

        return true;
    }
    catch (const std::exception &e)
    {
        // Clean up any allocated users in case of error
        for (User *user : users)
        {
            delete user;
        }
        users.clear();
        return false;
    }
}

bool NetworkParser::exportToJSON(const std::string &filename, const Graph &graph, const std::vector<User *> &users)
{
    try
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        file << "{\n  \"users\": [\n";

        for (size_t i = 0; i < users.size(); ++i)
        {
            const User *user = users[i];
            file << "    {\n";
            file << "      \"id\": \"" << user->getUserId() << "\",\n";
            file << "      \"name\": \"" << user->getName() << "\",\n";
            file << "      \"age\": " << user->getAge() << ",\n";
            file << "      \"location\": \"" << user->getLocation() << "\",\n";
            file << "      \"interests\": [\n";

            const auto &interests = user->getInterests();
            for (size_t j = 0; j < interests.size(); ++j)
            {
                file << "        \"" << interests[j] << "\"";
                if (j < interests.size() - 1)
                    file << ",";
                file << "\n";
            }

            file << "      ]\n";
            file << "    }";
            if (i < users.size() - 1)
                file << ",";
            file << "\n";
        }

        file << "  ],\n  \"connections\": [\n";

        // Export connections
        const auto &adjList = graph.getAdjacencyList();
        bool firstConnection = true;

        for (const auto &[userId, connections] : adjList)
        {
            for (const auto &connectedId : connections)
            {
                if (userId < connectedId)
                { // Only export each connection once
                    if (!firstConnection)
                        file << ",\n";
                    file << "    {\n";
                    file << "      \"user1\": \"" << userId << "\",\n";
                    file << "      \"user2\": \"" << connectedId << "\"\n";
                    file << "    }";
                    firstConnection = false;
                }
            }
        }

        file << "\n  ]\n}\n";
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

bool NetworkParser::exportToCSV(const std::string &filename, const Graph &graph, const std::vector<User *> &users)
{
    try
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        // Write header
        file << "id,name,age,location,interests\n";

        // Write user data
        for (const User *user : users)
        {
            file << user->getUserId() << ",";
            file << user->getName() << ",";
            file << user->getAge() << ",";
            file << user->getLocation() << ",";

            // Write interests as semicolon-separated list
            const auto &interests = user->getInterests();
            for (size_t i = 0; i < interests.size(); ++i)
            {
                file << interests[i];
                if (i < interests.size() - 1)
                    file << ";";
            }
            file << "\n";
        }

        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}