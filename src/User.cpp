#include "../include/User.hpp"
#include <sstream>
#include <algorithm>

User::User(const std::string &id, const std::string &userName, int userAge, const std::string &userLocation)
    : userId(id), name(userName), age(userAge), location(userLocation) {}

std::string User::getUserId() const
{
    return userId;
}

std::string User::getName() const
{
    return name;
}

int User::getAge() const
{
    return age;
}

std::string User::getLocation() const
{
    return location;
}

std::vector<std::string> User::getInterests() const
{
    return interests;
}

void User::setName(const std::string &newName)
{
    name = newName;
}

void User::setAge(int newAge)
{
    age = newAge;
}

void User::setLocation(const std::string &newLocation)
{
    location = newLocation;
}

void User::addInterest(const std::string &interest)
{
    for (const auto &existingInterest : interests)
    {
        if (existingInterest == interest)
            return;
    }
    interests.push_back(interest);
}

void User::removeInterest(const std::string &interest)
{
    auto it = std::find(interests.begin(), interests.end(), interest);
    if (it != interests.end())
    {
        interests.erase(it);
    }
}

void User::addProfileData(const std::string &key, const std::string &value)
{
    profileData[key] = value;
}

std::string User::getProfileData(const std::string &key) const
{
    auto it = profileData.find(key);
    return (it != profileData.end()) ? it->second : "";
}

bool User::hasProfileData(const std::string &key) const
{
    return profileData.find(key) != profileData.end();
}

std::string User::toString() const
{
    try
    {
        std::stringstream ss;
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
    catch (const std::exception &e)
    {
        return "Error formatting user data: " + std::string(e.what());
    }
}