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

int main()
{
    try
    {
        Graph socialNetwork;
        std::vector<User *> users;

        // Get current working directory and construct absolute path
        std::filesystem::path dataPath = std::filesystem::current_path() / "data" / "sample_network.json";
        std::cout << "Attempting to load from: " << dataPath << std::endl;

        // Load sample network data
        std::cout << "Loading social network data...\n";
        if (!NetworkParser::parseJSONFile(dataPath.string(), socialNetwork, users))
        {
            std::cerr << "Error loading network data!\n";
            return 1;
        }
        std::cout << "Network data loaded successfully!\n";
        std::cout << "Loaded " << users.size() << " users\n";

        // Print all users
        printDivider();
        printUserList(users, "All Users in the Network");

        // Demonstrate friend recommendations
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

        // Demonstrate community detection
        printDivider();
        auto communities = socialNetwork.detectCommunities(1);
        printCommunities(communities);

        // Demonstrate shortest paths
        printDivider();
        auto distances = socialNetwork.floydWarshall();
        printShortestPaths(distances, socialNetwork.getUsers());

        // Demonstrate profile searching
        printDivider();
        std::cout << "\nProfile Search Results:\n";

        // Use sets to prevent duplicate pointers in search results
        std::set<const User *> searchResults;

        // Search by name using KMP
        std::cout << "\nSearching for users with 'son' in their name (using KMP):\n";
        auto nameResults = StringSearch::searchUsersByName(users, "son", true);
        for (const User *user : nameResults)
        {
            searchResults.insert(user);
            std::cout << "Found: " << user->getName() << "\n";
        }

        // Search by location using Rabin-Karp
        std::cout << "\nSearching for users in cities containing 'san' (using Rabin-Karp):\n";
        auto locationResults = StringSearch::searchUsersByLocation(users, "san", false);
        for (const User *user : locationResults)
        {
            searchResults.insert(user);
            std::cout << "Found: " << user->getName() << " in " << user->getLocation() << "\n";
        }

        // Search by interest
        std::cout << "\nSearching for users interested in 'photography':\n";
        auto interestResults = StringSearch::searchUsersByInterest(users, "photography", true);
        for (const User *user : interestResults)
        {
            searchResults.insert(user);
            std::cout << "Found: " << user->getName() << "\n";
        }

        // Clear search results before cleanup
        searchResults.clear();

        // Clean up - only delete pointers from the original users vector
        for (User *user : users)
        {
            delete user;
        }
        users.clear();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}