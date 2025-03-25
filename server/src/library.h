#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

class Library {
public:
    Library();
    std::string getCatalog();
    std::string searchBooks(const std::string& query);
};

#endif // LIBRARY_H
