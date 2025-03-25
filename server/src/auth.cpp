#include "auth.h"
#include "utils.h"
#include <iostream>

bool loginUser(const std::string& username, const std::string& password) {
    json users = loadJson("Users.json");
    if (users.contains(username)) {
        std::string storedPassword = users[username]["password"];
        return (storedPassword == password);
    }
    return false;
}

bool registerUser(const std::string& username, const std::string& password) {
    json users = loadJson("Users.json");
    if (users.contains(username)) {
        return false;
    }
    // Create a new user with an empty checkedOutBooks array.
    users[username] = { {"password", password}, {"checkedOutBooks", json::array()} };
    saveJson("Users.json", users);
    return true;
}
