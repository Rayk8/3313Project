#include "library.h"
#include "utils.h"
#include <sstream>
#include <iostream>

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
