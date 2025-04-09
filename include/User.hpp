#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <unordered_map>

class User
{
private:
    std::string userId;
    std::string name;
    int age;
    std::string location;
    std::vector<std::string> interests;
    std::unordered_map<std::string, std::string> profileData;

public:
    User(const std::string &id, const std::string &userName, int userAge = 0,
         const std::string &userLocation = "");

    // Getters
    std::string getUserId() const;
    std::string getName() const;
    int getAge() const;
    std::string getLocation() const;
    std::vector<std::string> getInterests() const;

    // Setters
    void setName(const std::string &newName);
    void setAge(int newAge);
    void setLocation(const std::string &newLocation);
    void addInterest(const std::string &interest);
    void removeInterest(const std::string &interest);

    // Profile data management
    void addProfileData(const std::string &key, const std::string &value);
    std::string getProfileData(const std::string &key) const;
    bool hasProfileData(const std::string &key) const;

    // String representation
    std::string toString() const;
};

#endif // USER_HPP