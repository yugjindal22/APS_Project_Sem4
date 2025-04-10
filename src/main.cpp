#include <iostream>
#include <iomanip>
#include <limits>
#include <filesystem>
#include <set>
#include "../include/Graph.hpp"
#include "../include/User.hpp"
#include "../include/NetworkParser.hpp"
#include "../include/StringSearch.hpp"

using namespace std;

void printDivider()
{
    cout << "\n"
         << string(60, '-') << "\n";
}

void printUserList(const vector<User *> &users, const string &title)
{
    cout << "\n"
         << title << ":\n";
    for (const User *user : users)
    {
        cout << user->toString() << "\n";
    }
}

void printCommunities(const vector<vector<string>> &communities)
{
    cout << "\nDetected Communities:\n";
    for (size_t i = 0; i < communities.size(); ++i)
    {
        cout << "Community " << (i + 1) << ": ";
        for (size_t j = 0; j < communities[i].size(); ++j)
        {
            cout << communities[i][j];
            if (j < communities[i].size() - 1)
                cout << ", ";
        }
        cout << "\n";
    }
}

void printShortestPaths(const vector<vector<int>> &distances, const vector<string> &users)
{
    cout << "\nAll-Pairs Shortest Paths (Degrees of Separation):\n";
    cout << setw(15) << "To →";
    for (const auto &user : users)
    {
        cout << setw(8) << user;
    }
    cout << "\nFrom ↓\n";

    for (size_t i = 0; i < users.size(); ++i)
    {
        cout << setw(15) << users[i];
        for (size_t j = 0; j < users.size(); ++j)
        {
            if (distances[i][j] == numeric_limits<int>::max())
            {
                cout << setw(8) << "∞";
            }
            else
            {
                cout << setw(8) << distances[i][j];
            }
        }
        cout << "\n";
    }
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForEnter()
{
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getInput(const string &prompt)
{
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

int getIntInput(const string &prompt, int min = numeric_limits<int>::min(), int max = numeric_limits<int>::max())
{
    while (true)
    {
        cout << prompt;
        string input;
        getline(cin, input);

        try
        {
            int value = stoi(input);
            if (value >= min && value <= max)
            {
                return value;
            }
        }
        catch (...)
        {
        }

        cout << "Invalid input. Please try again.\n";
    }
}

void addNewUser(Graph &socialNetwork, vector<User *> &users)
{
    cout << "\n=== Add New User ===\n";
    string id = getInput("Enter user ID: ");
    string name = getInput("Enter name: ");
    int age = getIntInput("Enter age (1-120): ", 1, 120);
    string location = getInput("Enter location: ");

    User *user = new User(id, name, age, location);

    cout << "Add interests? (y/n): ";
    string answer;
    getline(cin, answer);

    while (answer == "y" || answer == "Y")
    {
        string interest = getInput("Enter an interest (or press Enter to finish): ");
        if (interest.empty())
            break;
        user->addInterest(interest);

        cout << "Add another interest? (y/n): ";
        getline(cin, answer);
    }

    users.push_back(user);
    socialNetwork.addUser(id);
    cout << "\nUser added successfully!\n";
}

void addConnection(Graph &socialNetwork, const vector<User *> &users)
{
    cout << "\n=== Add Connections ===\n";
    cout << "\nAvailable users:\n";
    for (const User *user : users)
    {
        cout << user->getUserId() << " - " << user->getName() << "\n";
    }

    vector<pair<string, string>> connections;
    string answer;

    do
    {
        string user1 = getInput("\nEnter first user ID: ");
        string user2 = getInput("Enter second user ID: ");

        bool user1Found = false, user2Found = false;
        for (const User *user : users)
        {
            if (user->getUserId() == user1)
                user1Found = true;
            if (user->getUserId() == user2)
                user2Found = true;
        }

        if (!user1Found || !user2Found)
        {
            cout << "One or both users not found!\n";
        }
        else
        {
            connections.push_back({user1, user2});
        }

        cout << "Add another connection? (y/n): ";
        getline(cin, answer);

    } while (answer == "y" || answer == "Y");

    // Add all connections at once
    for (const auto &[user1, user2] : connections)
    {
        socialNetwork.addConnection(user1, user2);
    }

    cout << "\n"
         << connections.size() << " connection(s) added successfully!\n";
}

void searchUsers(const vector<User *> &users)
{
    cout << "\n=== Search Users ===\n";
    cout << "1. Search by name\n";
    cout << "2. Search by location\n";
    cout << "3. Search by interest\n";

    int choice = getIntInput("Enter your choice (1-3): ", 1, 3);
    string searchTerm = getInput("Enter search term: ");

    cout << "\nSearch Results:\n";
    vector<User *> results;

    switch (choice)
    {
    case 1:
        results = StringSearch::searchUsersByName(users, searchTerm, true);
        break;
    case 2:
        results = StringSearch::searchUsersByLocation(users, searchTerm, false);
        break;
    case 3:
        results = StringSearch::searchUsersByInterest(users, searchTerm, true);
        break;
    }

    if (results.empty())
    {
        cout << "No matching users found.\n";
    }
    else
    {
        for (const User *user : results)
        {
            cout << "\n"
                 << user->toString();
        }
    }
}

void showMainMenu()
{
    cout << "\n=== Social Network Analyzer ===\n";
    cout << "1. Add new user\n";
    cout << "2. Add connection between users\n";
    cout << "3. Show all users\n";
    cout << "4. Show friend recommendations\n";
    cout << "5. Show communities\n";
    cout << "6. Show shortest paths\n";
    cout << "7. Search users\n";
    cout << "0. Exit\n";
}

void loadNetworkData(Graph &socialNetwork, vector<User *> &users)
{
    try
    {
        filesystem::path dataPath = filesystem::current_path() / "data" / "Network.json";
        cout << "\nLoading network data from: " << dataPath << endl;

        if (!filesystem::exists(dataPath))
        {
            cerr << "Error: Network.json file not found!\n";
            return;
        }

        if (!NetworkParser::parseJSONFile(dataPath.string(), socialNetwork, users))
        {
            cerr << "Error: Could not load network data!\n";
        }
        else
        {
            cout << "Network data fetched successfully!\n";
            cout << "Successfully loaded " << users.size() << " users\n";
        }
    }
    catch (const exception &e)
    {
        cerr << "Error loading network data: " << e.what() << endl;
    }
}

void saveNetworkData(const Graph &socialNetwork, const vector<User *> &users)
{
    filesystem::path dataPath = filesystem::current_path() / "data" / "Network.json";
    cout << "\nSaving network data to: " << dataPath << endl;

    if (!NetworkParser::exportToJSON(dataPath.string(), socialNetwork, users))
    {
        cerr << "Error: Failed to save network data!\n";
    }
    else
    {
        cout << "Network data saved successfully!\n";
    }
}

int main()
{
    Graph socialNetwork;
    vector<User *> users;

    // Automatically load data at startup
    loadNetworkData(socialNetwork, users);

    while (true)
    {
        clearScreen();
        showMainMenu();

        int choice = getIntInput("\nEnter your choice (0-7): ", 0, 7);

        if (choice == 0)
        {
            // Save data before exiting
            saveNetworkData(socialNetwork, users);
            break;
        }

        switch (choice)
        {
        case 1:
            addNewUser(socialNetwork, users);
            break;

        case 2:
            addConnection(socialNetwork, users);
            break;

        case 3:
            printDivider();
            printUserList(users, "All Users in the Network");
            break;

        case 4:
            printDivider();
            cout << "\nFriend Recommendations:\n";
            for (const User *user : users)
            {
                cout << "\nRecommendations for " << user->getName() << ":\n";
                auto recommendations = socialNetwork.getFriendRecommendations(user->getUserId());
                for (const auto &recommendedId : recommendations)
                {
                    for (const User *u : users)
                    {
                        if (u->getUserId() == recommendedId)
                        {
                            cout << "- " << u->getName() << "\n";
                            break;
                        }
                    }
                }
            }
            break;

        case 5:
            printDivider();
            printCommunities(socialNetwork.detectCommunities(1));
            break;

        case 6:
            printDivider();
            printShortestPaths(socialNetwork.floydWarshall(), socialNetwork.getUsers());
            break;

        case 7:
            searchUsers(users);
            break;
        }

        waitForEnter();
    }

    // Clean up
    for (User *user : users)
    {
        delete user;
    }
    users.clear();

    return 0;
}