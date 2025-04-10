#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class User
{
private:
    string userId;
    string name;
    int age;
    string location;
    vector<string> interests;
    unordered_map<string, string> profileData;

public:
    User(const string &id, const string &userName, int userAge = 0,
         const string &userLocation = "");

    // Getters
    string getUserId() const;
    string getName() const;
    int getAge() const;
    string getLocation() const;
    vector<string> getInterests() const;

    // Setters
    void setName(const string &newName);
    void setAge(int newAge);
    void setLocation(const string &newLocation);
    void addInterest(const string &interest);
    void removeInterest(const string &interest);

    // Profile data management
    void addProfileData(const string &key, const string &value);
    string getProfileData(const string &key) const;
    bool hasProfileData(const string &key) const;

    // String representation
    string toString() const;
};

#endif // USER_HPP