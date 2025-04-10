#ifndef STRING_SEARCH_HPP
#define STRING_SEARCH_HPP

#include <string>
#include <vector>
#include "User.hpp"

using namespace std;

class StringSearch
{
private:
    static const int PRIME = 101; // Prime number for Rabin-Karp
    static const int d = 256;     // Number of characters in alphabet

    // KMP helper function
    static vector<int> computeLPSArray(const string &pattern);

    // Rabin-Karp helper functions
    static long long calculateHash(const string &str, int end);
    static long long recalculateHash(long long oldHash, char oldChar, char newChar, long long h);
    static long long calculatePowerValue(int m);

public:
    // KMP (Knuth-Morris-Pratt) algorithm implementation
    static vector<size_t> KMPSearch(const string &text, const string &pattern);

    // Rabin-Karp algorithm implementation
    static vector<size_t> RabinKarpSearch(const string &text, const string &pattern);

    // Profile search functions
    static vector<User *> searchUsersByName(const vector<User *> &users, const string &searchPattern, bool useKMP = true);
    static vector<User *> searchUsersByLocation(const vector<User *> &users, const string &searchPattern, bool useKMP = true);
    static vector<User *> searchUsersByInterest(const vector<User *> &users, const string &searchPattern, bool useKMP = true);
    static vector<User *> searchUsersByProfileData(const vector<User *> &users, const string &key, const string &searchPattern, bool useKMP = true);
};

#endif // STRING_SEARCH_HPP