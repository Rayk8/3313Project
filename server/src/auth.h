#ifndef AUTH_H
#define AUTH_H

#include <string>

// Returns true if the provided username and password match an entry in Users.json.
bool loginUser(const std::string& username, const std::string& password);

// Registers a new user in Users.json.
// Returns true if registration is successful (i.e. user did not already exist), false otherwise.
bool registerUser(const std::string& username, const std::string& password);

#endif // AUTH_H
