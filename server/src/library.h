#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Book {
    std::string bookID;
    std::string title;
    bool available;
    std::string borrower;
    std::string image;
};

class Library {
public:
    Library();

    std::string getCatalog();
    std::string searchBooks(const std::string& query);
    std::string getCurrentBooks(const std::string& username);
    std::string returnBook(const std::string& username, const std::string& bookID);
    std::string borrowBook(const std::string& username, const std::string& bookID);

private:
    void loadBooks();
    void saveBooks();

    std::map<std::string, Book> books;
    std::mutex libraryMutex;
};

#endif // LIBRARY_H
