#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

class Library {
public:
    Library();
    std::string getCatalog();
    std::string searchBooks(const std::string& query);
    std::string getCurrentBooks(const std::string& username);
    std::string returnBook(const std::string& username, const std::string& bookID);
};

#endif // LIBRARY_H
