#include "library.h"
#include "utils.h"
#include <sstream>
#include <iostream>
#include <mutex>

std::mutex userMutex;

Library::Library() {
    // Initialization if needed.
}

std::string Library::getCatalog() {
    json books = loadJson("books.json");
    std::ostringstream oss;
    oss << "<html><body><h1>Book Catalog</h1><ul>";
    for (auto it = books.begin(); it != books.end(); ++it) {
        std::string bookID = it.key();
        std::string title = it.value()["title"];
        bool available = it.value()["available"];
        oss << "<li>" << title << " (ID: " << bookID << ") - " 
            << (available ? "Available" : "Checked Out") << "</li>";
    }
    oss << "</ul></body></html>";
    return oss.str();
}

std::string Library::searchBooks(const std::string& query) {
    json books = loadJson("books.json");
    std::ostringstream oss;
    oss << "<html><body><h1>Search Results</h1><ul>";
    for (auto it = books.begin(); it != books.end(); ++it) {
        std::string bookID = it.key();
        std::string title = it.value()["title"];
        bool available = it.value()["available"];
        if (title.find(query) != std::string::npos) {  // simple substring search
            oss << "<li>" << title << " (ID: " << bookID << ") - " 
                << (available ? "Available" : "Checked Out") << "</li>";
        }
    }
    oss << "</ul></body></html>";
    return oss.str();
}

std::string Library::getCurrentBooks(const std::string& username) {
    std::lock_guard<std::mutex> lock(userMutex);
    json users = loadJson("Users.json");
    json books = loadJson("books.json");

    std::ostringstream oss;
    
    if (users.contains(username)) {
        for (const auto& bookID : users[username]["checkedOutBooks"]) {
            std::string title = books[bookID]["title"];
            oss << "<div class='book-container'>"
                << "<h3>" << title << " (ID: " << bookID << ")</h3>"
                << "<button onclick=\"checkInBook('" << bookID << "')\">Check In</button>"
                << "</div>";
        }
    } else {
        oss << "<p>User not found</p>";
    }
    return oss.str();
}
