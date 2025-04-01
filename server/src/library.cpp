#include "library.h"
#include "utils.h"
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <ctime>

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
            std::string id = bookID.get<std::string>();
            std::string title = books[id]["title"];
            oss << "<div class='book-container'>"
                << "<h3>" << title << " (ID: " << id << ")</h3>"
                << "<button onclick=\"checkInBook('" << id << "')\">Check In</button>"
                << "</div>";
        }
    } else {
        oss << "<p>User not found</p>";
    }
    return oss.str();
}

std::string Library::returnBook(const std::string& username, const std::string& bookID) {
    std::lock_guard<std::mutex> lock(userMutex);

    json users = loadJson("Users.json");
    json books = loadJson("books.json");
    json transactions = loadJson("Transactions.json");

    if (!users.contains(username)) {
        return "User not found.";
    }

    auto& checkedOut = users[username]["checkedOutBooks"];
    auto it = std::find(checkedOut.begin(), checkedOut.end(), bookID);
    if (it == checkedOut.end()) {
        return "Book not currently checked out.";
    }

    // Get book title
    std::string title = books.contains(bookID) ? books[bookID]["title"] : "Unknown";

    // Remove from user's checked out books
    checkedOut.erase(it);

    // Mark book as available
    if (books.contains(bookID)) {
        books[bookID]["available"] = true;
    }

    // Log transaction with timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::gmtime(&now_time), "%FT%TZ");

    transactions[username].push_back({
        {"bookID", bookID},
        {"title", title},
        {"action", "return"},
        {"timestamp", timestamp.str()}
    });

    // Save everything atomically
    saveJson("Users.json", users);
    saveJson("books.json", books);
    saveJson("Transactions.json", transactions);

    return "Book successfully checked in.";
}