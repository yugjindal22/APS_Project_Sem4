#include "../include/User.hpp"
#include <sstream>
#include <algorithm>

User::User(const string &id, const string &userName, int userAge, const string &userLocation)
    : userId(id), name(userName), age(userAge), location(userLocation) {}

string User::getUserId() const
{
    return userId;
}

string User::getName() const
{
    return name;
}

int User::getAge() const
{
    return age;
}

string User::getLocation() const
{
    return location;
}

vector<string> User::getInterests() const
{
    return interests;
}

void User::setName(const string &newName)
{
    name = newName;
}

void User::setAge(int newAge)
{
    age = newAge;
}

void User::setLocation(const string &newLocation)
{
    location = newLocation;
}

void User::addInterest(const string &interest)
{
    for (const auto &existingInterest : interests)
    {
        if (existingInterest == interest)
            return;
    }
    interests.push_back(interest);
}

void User::removeInterest(const string &interest)
{
    auto it = find(interests.begin(), interests.end(), interest);
    if (it != interests.end())
    {
        interests.erase(it);
    }
}

void User::addProfileData(const string &key, const string &value)
{
    profileData[key] = value;
}

string User::getProfileData(const string &key) const
{
    auto it = profileData.find(key);
    return (it != profileData.end()) ? it->second : "";
}

bool User::hasProfileData(const string &key) const
{
    return profileData.find(key) != profileData.end();
}

string User::toString() const
{
    try
    {
        stringstream ss;
        ss << "User ID: " << (userId.empty() ? "N/A" : userId) << "\n"
           << "Name: " << (name.empty() ? "N/A" : name) << "\n"
           << "Age: " << age << "\n"
           << "Location: " << (location.empty() ? "N/A" : location) << "\n"
           << "Interests: ";

        if (interests.empty())
        {
            ss << "None";
        }
        else
        {
            for (size_t i = 0; i < interests.size(); ++i)
            {
                if (!interests[i].empty())
                {
                    ss << interests[i];
                    if (i < interests.size() - 1)
                        ss << ", ";
                }
            }
        }
        ss << "\n";

        if (!profileData.empty())
        {
            ss << "Additional Profile Data:\n";
            for (const auto &[key, value] : profileData)
            {
                if (!key.empty())
                {
                    ss << "  " << key << ": " << (value.empty() ? "N/A" : value) << "\n";
                }
            }
        }

        return ss.str();
    }
    catch (const exception &e)
    {
        return "Error formatting user data: " + string(e.what());
    }
}