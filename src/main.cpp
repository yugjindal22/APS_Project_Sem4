#include <iostream>
#include <iomanip>
#include <limits>
#include <filesystem>
#include <set>
#include "../include/Graph.hpp"
#include "../include/User.hpp"
#include "../include/NetworkParser.hpp"
#include "../include/StringSearch.hpp"

void printDivider()
{
    std::cout << "\n"
              << std::string(60, '-') << "\n";
}

void printUserList(const std::vector<User *> &users, const std::string &title)
{
    std::cout << "\n"
              << title << ":\n";
    for (const User *user : users)
    {
        std::cout << user->toString() << "\n";
    }
}

void printCommunities(const std::vector<std::vector<std::string>> &communities)
{
    std::cout << "\nDetected Communities:\n";
    for (size_t i = 0; i < communities.size(); ++i)
    {
        std::cout << "Community " << (i + 1) << ": ";
        for (size_t j = 0; j < communities[i].size(); ++j)
        {
            std::cout << communities[i][j];
            if (j < communities[i].size() - 1)
                std::cout << ", ";
        }
        std::cout << "\n";
    }
}

void printShortestPaths(const std::vector<std::vector<int>> &distances, const std::vector<std::string> &users)
{
    std::cout << "\nAll-Pairs Shortest Paths (Degrees of Separation):\n";
    std::cout << std::setw(15) << "To →";
    for (const auto &user : users)
    {
        std::cout << std::setw(8) << user;
    }
    std::cout << "\nFrom ↓\n";

    for (size_t i = 0; i < users.size(); ++i)
    {
        std::cout << std::setw(15) << users[i];
        for (size_t j = 0; j < users.size(); ++j)
        {
            if (distances[i][j] == std::numeric_limits<int>::max())
            {
                std::cout << std::setw(8) << "∞";
            }
            else
            {
                std::cout << std::setw(8) << distances[i][j];
            }
        }
        std::cout << "\n";
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
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getInput(const std::string &prompt)
{
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int getIntInput(const std::string &prompt, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max())
{
    while (true)
    {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try
        {
            int value = std::stoi(input);
            if (value >= min && value <= max)
            {
                return value;
            }
        }
        catch (...)
        {
        }

        std::cout << "Invalid input. Please try again.\n";
    }
}

void addNewUser(Graph &socialNetwork, std::vector<User *> &users)
{
    std::cout << "\n=== Add New User ===\n";
    std::string id = getInput("Enter user ID: ");
    std::string name = getInput("Enter name: ");
    int age = getIntInput("Enter age (1-120): ", 1, 120);
    std::string location = getInput("Enter location: ");

    User *user = new User(id, name, age, location);

    std::cout << "Add interests? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);

    while (answer == "y" || answer == "Y")
    {
        std::string interest = getInput("Enter an interest (or press Enter to finish): ");
        if (interest.empty())
            break;
        user->addInterest(interest);

        std::cout << "Add another interest? (y/n): ";
        std::getline(std::cin, answer);
    }

    users.push_back(user);
    socialNetwork.addUser(id);
    std::cout << "\nUser added successfully!\n";
}

void addConnection(Graph &socialNetwork, const std::vector<User *> &users)
{
    std::cout << "\n=== Add Connections ===\n";
    std::cout << "\nAvailable users:\n";
    for (const User *user : users)
    {
        std::cout << user->getUserId() << " - " << user->getName() << "\n";
    }

    std::vector<std::pair<std::string, std::string>> connections;
    std::string answer;

    do
    {
        std::string user1 = getInput("\nEnter first user ID: ");
        std::string user2 = getInput("Enter second user ID: ");

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
            std::cout << "One or both users not found!\n";
        }
        else
        {
            connections.push_back({user1, user2});
        }

        std::cout << "Add another connection? (y/n): ";
        std::getline(std::cin, answer);

    } while (answer == "y" || answer == "Y");

    // Add all connections at once
    for (const auto &[user1, user2] : connections)
    {
        socialNetwork.addConnection(user1, user2);
    }

    std::cout << "\n"
              << connections.size() << " connection(s) added successfully!\n";
}

void searchUsers(const std::vector<User *> &users)
{
    std::cout << "\n=== Search Users ===\n";
    std::cout << "1. Search by name\n";
    std::cout << "2. Search by location\n";
    std::cout << "3. Search by interest\n";

    int choice = getIntInput("Enter your choice (1-3): ", 1, 3);
    std::string searchTerm = getInput("Enter search term: ");

    std::cout << "\nSearch Results:\n";
    std::vector<User *> results;

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
        std::cout << "No matching users found.\n";
    }
    else
    {
        for (const User *user : results)
        {
            std::cout << "\n"
                      << user->toString();
        }
    }
}

void showMainMenu()
{
    std::cout << "\n=== Social Network Analyzer ===\n";
    std::cout << "1. Add new user\n";
    std::cout << "2. Add connection between users\n";
    std::cout << "3. Show all users\n";
    std::cout << "4. Show friend recommendations\n";
    std::cout << "5. Show communities\n";
    std::cout << "6. Show shortest paths\n";
    std::cout << "7. Search users\n";
    std::cout << "0. Exit\n";
}

void loadNetworkData(Graph &socialNetwork, std::vector<User *> &users)
{
    try
    {
        std::filesystem::path dataPath = std::filesystem::current_path() / "data" / "Network.json";
        std::cout << "\nLoading network data from: " << dataPath << std::endl;

        if (!std::filesystem::exists(dataPath))
        {
            std::cerr << "Error: Network.json file not found!\n";
            return;
        }

        if (!NetworkParser::parseJSONFile(dataPath.string(), socialNetwork, users))
        {
            std::cerr << "Error: Could not load network data!\n";
        }
        else
        {
            std::cout << "Network data fetched successfully!\n";
            std::cout << "Successfully loaded " << users.size() << " users\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading network data: " << e.what() << std::endl;
    }
}

void saveNetworkData(const Graph &socialNetwork, const std::vector<User *> &users)
{
    std::filesystem::path dataPath = std::filesystem::current_path() / "data" / "Network.json";
    std::cout << "\nSaving network data to: " << dataPath << std::endl;

    if (!NetworkParser::exportToJSON(dataPath.string(), socialNetwork, users))
    {
        std::cerr << "Error: Failed to save network data!\n";
    }
    else
    {
        std::cout << "Network data saved successfully!\n";
    }
}

int main()
{
    Graph socialNetwork;
    std::vector<User *> users;

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
            std::cout << "\nFriend Recommendations:\n";
            for (const User *user : users)
            {
                std::cout << "\nRecommendations for " << user->getName() << ":\n";
                auto recommendations = socialNetwork.getFriendRecommendations(user->getUserId());
                for (const auto &recommendedId : recommendations)
                {
                    for (const User *u : users)
                    {
                        if (u->getUserId() == recommendedId)
                        {
                            std::cout << "- " << u->getName() << "\n";
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