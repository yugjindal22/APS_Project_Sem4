#ifndef STRING_SEARCH_HPP
#define STRING_SEARCH_HPP

#include <string>
#include <vector>
#include "User.hpp"

class StringSearch
{
private:
    static const int PRIME = 101; // Prime number for Rabin-Karp
    static const int d = 256;     // Number of characters in alphabet

    // KMP helper function
    static std::vector<int> computeLPSArray(const std::string &pattern);

    // Rabin-Karp helper functions
    static long long calculateHash(const std::string &str, int end);
    static long long recalculateHash(long long oldHash, char oldChar, char newChar, long long h);
    static long long calculatePowerValue(int m);

public:
    // KMP (Knuth-Morris-Pratt) algorithm implementation
    static std::vector<size_t> KMPSearch(const std::string &text, const std::string &pattern);

    // Rabin-Karp algorithm implementation
    static std::vector<size_t> RabinKarpSearch(const std::string &text, const std::string &pattern);

    // Profile search functions
    static std::vector<User *> searchUsersByName(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP = true);
    static std::vector<User *> searchUsersByLocation(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP = true);
    static std::vector<User *> searchUsersByInterest(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP = true);
    static std::vector<User *> searchUsersByProfileData(const std::vector<User *> &users, const std::string &key, const std::string &searchPattern, bool useKMP = true);
};

#endif // STRING_SEARCH_HPP