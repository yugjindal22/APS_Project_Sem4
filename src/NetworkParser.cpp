#include "../include/NetworkParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

string NetworkParser::readFile(const string &filename)
{
    cout << "Attempting to read file: " << filename << endl;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        throw runtime_error("Could not open file: " + filename);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    cout << "Successfully read " << content.length() << " bytes" << endl;
    return content;
}

void NetworkParser::skipWhitespace(const string &jsonStr, size_t &pos)
{
    while (pos < jsonStr.length() && (isspace(jsonStr[pos]) || jsonStr[pos] == '\n' || jsonStr[pos] == '\r'))
    {
        pos++;
    }
}

string NetworkParser::parseString(const string &jsonStr, size_t &pos)
{
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length())
    {
        throw runtime_error("Unexpected end of input while parsing string");
    }

    if (jsonStr[pos] != '"')
    {
        cerr << "Debug: Expected '\"' at position " << pos << ", found '" << jsonStr[pos] << "'\n";
        throw runtime_error("Expected string at position " + to_string(pos));
    }

    string result;
    pos++; // Skip opening quote

    while (pos < jsonStr.length() && jsonStr[pos] != '"')
    {
        if (jsonStr[pos] == '\\')
        {
            pos++;
            if (pos >= jsonStr.length())
            {
                throw runtime_error("Unterminated escape sequence");
            }
            switch (jsonStr[pos])
            {
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case '/':
                result += '/';
                break;
            case 'b':
                result += '\b';
                break;
            case 'f':
                result += '\f';
                break;
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            default:
                result += jsonStr[pos];
            }
        }
        else
        {
            result += jsonStr[pos];
        }
        pos++;
    }

    if (pos >= jsonStr.length())
    {
        throw runtime_error("Unterminated string");
    }

    pos++; // Skip closing quote
    return result;
}

int NetworkParser::parseNumber(const string &jsonStr, size_t &pos)
{
    skipWhitespace(jsonStr, pos);

    size_t start = pos;
    while (pos < jsonStr.length() && (isdigit(jsonStr[pos]) || jsonStr[pos] == '-'))
    {
        pos++;
    }

    return stoi(jsonStr.substr(start, pos - start));
}

void NetworkParser::parseUserData(const string &jsonStr, size_t &pos, User *&user)
{
    cout << "Debug: Starting to parse user data at position " << pos << "\n";
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length() || jsonStr[pos] != '{')
    {
        cerr << "Debug: Expected '{' at position " << pos << ", found '"
             << (pos < jsonStr.length() ? jsonStr[pos] : '?') << "'\n";
        throw runtime_error("Expected object at position " + to_string(pos));
    }

    pos++; // Skip {
    cout << "Debug: Found opening brace for user object\n";

    string id, name, location;
    int age = 0;
    vector<string> interests;

    while (pos < jsonStr.length() && jsonStr[pos] != '}')
    {
        skipWhitespace(jsonStr, pos);
        cout << "Debug: Parsing user field at position " << pos << "\n";

        string key = parseString(jsonStr, pos);
        cout << "Debug: Found user field: " << key << "\n";

        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.length() || jsonStr[pos] != ':')
        {
            throw runtime_error("Expected ':' at position " + to_string(pos));
        }
        pos++; // Skip :
        skipWhitespace(jsonStr, pos);

        if (key == "id")
        {
            id = parseString(jsonStr, pos);
            cout << "Debug: Parsed user ID: " << id << "\n";
        }
        else if (key == "name")
        {
            name = parseString(jsonStr, pos);
            cout << "Debug: Parsed user name: " << name << "\n";
        }
        else if (key == "age")
        {
            age = parseNumber(jsonStr, pos);
            cout << "Debug: Parsed user age: " << age << "\n";
        }
        else if (key == "location")
        {
            location = parseString(jsonStr, pos);
            cout << "Debug: Parsed user location: " << location << "\n";
        }
        else if (key == "interests")
        {
            cout << "Debug: Starting to parse interests array\n";
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.length() || jsonStr[pos] != '[')
            {
                throw runtime_error("Expected array at position " + to_string(pos));
            }
            pos++; // Skip [
            skipWhitespace(jsonStr, pos);

            // Handle empty array case
            if (jsonStr[pos] != ']')
            {
                while (pos < jsonStr.length() && jsonStr[pos] != ']')
                {
                    skipWhitespace(jsonStr, pos);
                    string interest = parseString(jsonStr, pos);
                    interests.push_back(interest);
                    cout << "Debug: Added interest: " << interest << "\n";

                    skipWhitespace(jsonStr, pos);
                    if (jsonStr[pos] == ',')
                        pos++;
                }
            }

            if (pos >= jsonStr.length() || jsonStr[pos] != ']')
            {
                throw runtime_error("Unterminated array");
            }
            pos++; // Skip ]
            cout << "Debug: Finished parsing interests array with " << interests.size() << " interests\n";
        }

        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] == ',')
            pos++;
    }

    if (pos >= jsonStr.length() || jsonStr[pos] != '}')
    {
        throw runtime_error("Unterminated object");
    }
    pos++; // Skip }

    cout << "Debug: Creating user object\n";
    user = new User(id, name, age, location);
    for (const auto &interest : interests)
    {
        user->addInterest(interest);
    }
    cout << "Debug: Successfully created user object\n";
}

void NetworkParser::parseConnections(const string &jsonStr, size_t &pos, Graph &graph)
{
    skipWhitespace(jsonStr, pos);

    if (pos >= jsonStr.length() || jsonStr[pos] != '[')
    {
        throw runtime_error("Expected array at position " + to_string(pos));
    }

    pos++; // Skip [

    vector<pair<string, string>> connections;

    while (pos < jsonStr.length() && jsonStr[pos] != ']')
    {
        skipWhitespace(jsonStr, pos);

        if (jsonStr[pos] != '{')
        {
            throw runtime_error("Expected object at position " + to_string(pos));
        }

        pos++; // Skip {

        string user1, user2;

        while (pos < jsonStr.length() && jsonStr[pos] != '}')
        {
            skipWhitespace(jsonStr, pos);

            string key = parseString(jsonStr, pos);
            skipWhitespace(jsonStr, pos);

            if (pos >= jsonStr.length() || jsonStr[pos] != ':')
            {
                throw runtime_error("Expected ':' at position " + to_string(pos));
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
            throw runtime_error("Unterminated object");
        }
        pos++; // Skip }

        if (!user1.empty() && !user2.empty())
        {
            connections.push_back({user1, user2});
        }

        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] == ',')
            pos++;
    }

    if (pos >= jsonStr.length() || jsonStr[pos] != ']')
    {
        throw runtime_error("Unterminated array");
    }
    pos++; // Skip ]

    // Add all connections at once
    for (const auto &[user1, user2] : connections)
    {
        graph.addConnection(user1, user2);
    }
}

bool NetworkParser::parseJSONFile(const string &filename, Graph &graph, vector<User *> &users)
{
    try
    {
        cout << "Debug: Starting JSON parsing...\n";
        string jsonStr = readFile(filename);
        size_t pos = 0;

        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.length() || jsonStr[pos] != '{')
        {
            cerr << "Debug: Expected object at start of file\n";
            throw runtime_error("Expected object at start of file");
        }
        pos++; // Skip {
        cout << "Debug: Found opening brace\n";

        while (pos < jsonStr.length() && jsonStr[pos] != '}')
        {
            skipWhitespace(jsonStr, pos);
            cout << "Debug: Parsing key at position " << pos << "\n";

            string key = parseString(jsonStr, pos);
            cout << "Debug: Found key: " << key << "\n";

            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.length() || jsonStr[pos] != ':')
            {
                cerr << "Debug: Expected ':' after key '" << key << "'\n";
                throw runtime_error("Expected ':' at position " + to_string(pos));
            }
            pos++; // Skip :

            if (key == "users")
            {
                cout << "Debug: Starting to parse users array\n";
                skipWhitespace(jsonStr, pos);
                if (jsonStr[pos] != '[')
                {
                    cerr << "Debug: Expected '[' for users array\n";
                    throw runtime_error("Expected array at position " + to_string(pos));
                }
                pos++; // Skip [

                while (pos < jsonStr.length() && jsonStr[pos] != ']')
                {
                    cout << "Debug: Parsing user object\n";
                    User *user = nullptr;
                    parseUserData(jsonStr, pos, user);
                    if (user)
                    {
                        cout << "Debug: Successfully parsed user: " << user->getName() << "\n";
                        users.push_back(user);
                        graph.addUser(user->getUserId());
                    }
                    skipWhitespace(jsonStr, pos);
                    if (jsonStr[pos] == ',')
                        pos++;
                }

                if (pos >= jsonStr.length() || jsonStr[pos] != ']')
                {
                    cerr << "Debug: Users array not properly terminated\n";
                    throw runtime_error("Unterminated array");
                }
                pos++; // Skip ]
                cout << "Debug: Finished parsing users array\n";
            }
            else if (key == "connections")
            {
                cout << "Debug: Starting to parse connections\n";
                parseConnections(jsonStr, pos, graph);
                cout << "Debug: Finished parsing connections\n";
            }

            skipWhitespace(jsonStr, pos);
            if (jsonStr[pos] == ',')
                pos++;
        }

        if (pos >= jsonStr.length() || jsonStr[pos] != '}')
        {
            cerr << "Debug: JSON object not properly terminated\n";
            throw runtime_error("Unterminated object");
        }

        cout << "Debug: JSON parsing completed successfully\n";
        return true;
    }
    catch (const exception &e)
    {
        cerr << "Debug: Exception caught: " << e.what() << "\n";
        // Clean up any allocated users in case of error
        for (User *user : users)
        {
            delete user;
        }
        users.clear();
        return false;
    }
}

bool NetworkParser::parseCSVFile(const string &filename, Graph &graph, vector<User *> &users)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    try
    {
        string line;
        bool isFirstLine = true;
        vector<string> headers;

        while (getline(file, line))
        {
            if (isFirstLine)
            {
                stringstream ss(line);
                string header;
                while (getline(ss, header, ','))
                {
                    headers.push_back(header);
                }
                isFirstLine = false;
                continue;
            }

            stringstream ss(line);
            string value;
            vector<string> values;

            while (getline(ss, value, ','))
            {
                values.push_back(value);
            }

            if (values.size() >= 4)
            { // Assuming minimum required fields: id, name, age, location
                User *user = new User(values[0], values[1], stoi(values[2]), values[3]);
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
    catch (const exception &e)
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

bool NetworkParser::exportToJSON(const string &filename, const Graph &graph, const vector<User *> &users)
{
    try
    {
        ofstream file(filename);
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
    catch (const exception &e)
    {
        return false;
    }
}

bool NetworkParser::exportToCSV(const string &filename, const Graph &graph, const vector<User *> &users)
{
    try
    {
        ofstream file(filename);
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
    catch (const exception &e)
    {
        return false;
    }
}