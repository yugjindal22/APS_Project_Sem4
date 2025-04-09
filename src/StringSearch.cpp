#include "../include/StringSearch.hpp"
#include <algorithm>

// Helper function to convert string to lowercase
std::string toLowerCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<int> StringSearch::computeLPSArray(const std::string &pattern)
{
    int m = pattern.length();
    std::vector<int> lps(m, 0);

    int len = 0;
    int i = 1;

    while (i < m)
    {
        if (pattern[i] == pattern[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

std::vector<size_t> StringSearch::KMPSearch(const std::string &text, const std::string &pattern)
{
    std::vector<size_t> positions;
    if (pattern.empty() || text.empty())
        return positions;

    // Convert both strings to lowercase for case-insensitive comparison
    std::string lowerText = toLowerCase(text);
    std::string lowerPattern = toLowerCase(pattern);

    std::vector<int> lps(lowerPattern.length(), 0);

    // Compute LPS array
    int len = 0;
    int i = 1;
    while (i < lowerPattern.length())
    {
        if (lowerPattern[i] == lowerPattern[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }

    // Find pattern matches
    i = 0;
    int j = 0;
    while (i < lowerText.length())
    {
        if (lowerPattern[j] == lowerText[i])
        {
            j++;
            i++;
        }

        if (j == lowerPattern.length())
        {
            positions.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < lowerText.length() && lowerPattern[j] != lowerText[i])
        {
            if (j != 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i++;
            }
        }
    }

    return positions;
}

long long StringSearch::calculateHash(const std::string &str, int end)
{
    long long hash = 0;
    for (int i = 0; i <= end; i++)
    {
        hash = (hash * d + str[i]) % PRIME;
    }
    return hash;
}

long long StringSearch::recalculateHash(long long oldHash, char oldChar, char newChar, long long h)
{
    long long newHash = (oldHash - oldChar * h) % PRIME;
    newHash = (newHash * d + newChar) % PRIME;
    if (newHash < 0)
        newHash += PRIME;
    return newHash;
}

long long StringSearch::calculatePowerValue(int m)
{
    long long h = 1;
    for (int i = 0; i < m - 1; i++)
    {
        h = (h * d) % PRIME;
    }
    return h;
}

std::vector<size_t> StringSearch::RabinKarpSearch(const std::string &text, const std::string &pattern)
{
    std::vector<size_t> positions;
    if (pattern.empty() || text.empty())
        return positions;

    // Convert both strings to lowercase for case-insensitive comparison
    std::string lowerText = toLowerCase(text);
    std::string lowerPattern = toLowerCase(pattern);

    const int prime = 101;
    const int d = 256;
    int h = 1;
    int patternHash = 0;
    int textHash = 0;

    for (int i = 0; i < lowerPattern.length() - 1; i++)
    {
        h = (h * d) % prime;
    }

    for (int i = 0; i < lowerPattern.length(); i++)
    {
        patternHash = (d * patternHash + lowerPattern[i]) % prime;
        textHash = (d * textHash + lowerText[i]) % prime;
    }

    for (int i = 0; i <= lowerText.length() - lowerPattern.length(); i++)
    {
        if (patternHash == textHash)
        {
            bool match = true;
            for (int j = 0; j < lowerPattern.length(); j++)
            {
                if (lowerText[i + j] != lowerPattern[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                positions.push_back(i);
            }
        }

        if (i < lowerText.length() - lowerPattern.length())
        {
            textHash = (d * (textHash - lowerText[i] * h) + lowerText[i + lowerPattern.length()]) % prime;
            if (textHash < 0)
            {
                textHash += prime;
            }
        }
    }

    return positions;
}

std::vector<User *> StringSearch::searchUsersByName(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP)
{
    std::vector<User *> results;
    for (User *user : users)
    {
        std::string name = user->getName();
        std::vector<size_t> matches = useKMP ? KMPSearch(name, searchPattern) : RabinKarpSearch(name, searchPattern);

        if (!matches.empty())
        {
            results.push_back(user);
        }
    }
    return results;
}

std::vector<User *> StringSearch::searchUsersByLocation(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP)
{
    std::vector<User *> results;
    for (User *user : users)
    {
        std::string location = user->getLocation();
        std::vector<size_t> matches = useKMP ? KMPSearch(location, searchPattern) : RabinKarpSearch(location, searchPattern);

        if (!matches.empty())
        {
            results.push_back(user);
        }
    }
    return results;
}

std::vector<User *> StringSearch::searchUsersByInterest(const std::vector<User *> &users, const std::string &searchPattern, bool useKMP)
{
    std::vector<User *> results;
    for (User *user : users)
    {
        bool found = false;
        for (const std::string &interest : user->getInterests())
        {
            std::vector<size_t> matches = useKMP ? KMPSearch(interest, searchPattern) : RabinKarpSearch(interest, searchPattern);

            if (!matches.empty())
            {
                results.push_back(user);
                found = true;
                break;
            }
        }
    }
    return results;
}

std::vector<User *> StringSearch::searchUsersByProfileData(const std::vector<User *> &users, const std::string &key, const std::string &searchPattern, bool useKMP)
{
    std::vector<User *> results;
    for (User *user : users)
    {
        if (user->hasProfileData(key))
        {
            std::string value = user->getProfileData(key);
            std::vector<size_t> matches = useKMP ? KMPSearch(value, searchPattern) : RabinKarpSearch(value, searchPattern);

            if (!matches.empty())
            {
                results.push_back(user);
            }
        }
    }
    return results;
}