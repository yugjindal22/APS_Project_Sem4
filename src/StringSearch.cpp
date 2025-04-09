#include "../include/StringSearch.hpp"
#include <algorithm>

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
    int n = text.length();
    int m = pattern.length();

    if (m == 0 || n == 0)
        return positions;

    std::vector<int> lps = computeLPSArray(pattern);

    int i = 0; // index for text
    int j = 0; // index for pattern

    while (i < n)
    {
        if (pattern[j] == text[i])
        {
            j++;
            i++;
        }

        if (j == m)
        {
            positions.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i])
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
    int n = text.length();
    int m = pattern.length();

    if (m == 0 || n == 0 || m > n)
        return positions;

    long long patternHash = calculateHash(pattern, m - 1);
    long long textHash = calculateHash(text, m - 1);
    long long h = calculatePowerValue(m);

    for (int i = 0; i <= n - m; i++)
    {
        if (patternHash == textHash)
        {
            bool match = true;
            for (int j = 0; j < m; j++)
            {
                if (text[i + j] != pattern[j])
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

        if (i < n - m)
        {
            textHash = recalculateHash(textHash, text[i], text[i + m], h);
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