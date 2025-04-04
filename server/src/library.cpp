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
    loadBooks(); 
}

void Library::loadBooks() {
    json bookData = loadJson("books.json");

    for (auto& [bookID, info] : bookData.items()) {
        Book b;
        b.bookID = bookID;
        b.title = info["title"];
        b.available = info["available"];
        b.borrower = info["borrower"];
        books[bookID] = b;
    }
}

std::string Library::getCatalog() {
    std::lock_guard<std::mutex> lock(libraryMutex);
    std::string html = "<div style='display: flex; flex-wrap: wrap; gap: 1rem;'>";

    for (const auto& pair : books) {
        const Book& book = pair.second;

        html += "<div style='border: 1px solid #ccc; padding: 1rem; width: 250px; border-radius: 8px; box-shadow: 0 2px 6px rgba(0,0,0,0.1);'>";
        html += "<h3>" + book.title + "</h3>";
        html += "<p><strong>ID:</strong> " + book.bookID + "</p>";

        if (book.available) {
            html += "<button onclick=\"checkoutBook('" + book.bookID + "')\">Borrow</button>";
        } else {
            html += "<button disabled>Checked out by " + book.borrower + "</button>";
        }

        html += "</div>\n";
    }

    html += "</div>";
    return html;
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

std::string Library::borrowBook(const std::string& username, const std::string& bookID) {
    std::lock_guard<std::mutex> lock(libraryMutex);

    json users = loadJson("Users.json");
    json booksData = loadJson("books.json");
    json transactions = loadJson("Transactions.json");

    if (!booksData.contains(bookID)) {
        return "Book not found.";
    }

    if (!booksData[bookID]["available"]) {
        return "Book is already checked out by " + booksData[bookID]["borrower"].get<std::string>();
    }

    // update book data
    booksData[bookID]["available"] = false;
    booksData[bookID]["borrower"] = username;

    // update user data
    if (!users.contains(username)) {
        users[username] = { {"checkedOutBooks", json::array()} };
    }

    users[username]["checkedOutBooks"].push_back(bookID);

    // log transaction
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::gmtime(&now_time), "%FT%TZ");

    transactions[username].push_back({
        {"bookID", bookID},
        {"title", booksData[bookID]["title"]},
        {"action", "borrow"},
        {"timestamp", timestamp.str()}
    });

    // save all changes
    saveJson("books.json", booksData);
    saveJson("Users.json", users);
    saveJson("Transactions.json", transactions);

    // also update in-memory map
    if (books.find(bookID) != books.end()) {
        books[bookID].available = false;
        books[bookID].borrower = username;
    }

    return "Successfully borrowed '" + booksData[bookID]["title"].get<std::string>() + "'";
}

std::string Library::returnBook(const std::string& username, const std::string& bookID) {
    std::lock_guard<std::mutex> lock(userMutex);

    json users = loadJson("Users.json");
    json books = loadJson("books.json");
    json transactions = loadJson("Transactions.json");

    if (!users.contains(username)) return "User not found.";

    auto& checkedOut = users[username]["checkedOutBooks"];
    auto it = std::find(checkedOut.begin(), checkedOut.end(), bookID);
    if (it == checkedOut.end()) return "Book not currently checked out.";

    std::string title = books.contains(bookID) ? books[bookID]["title"] : "Unknown";

    // remove from user
    checkedOut.erase(it);

    // mark book available
    if (books.contains(bookID)) {
        books[bookID]["available"] = true;
        books[bookID]["borrower"] = "";  // <- This part is key
    }

    // log transaction
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

    // save changes
    saveJson("Users.json", users);
    saveJson("books.json", books);
    saveJson("Transactions.json", transactions);

    // udate in-memory map
    if (this->books.find(bookID) != this->books.end()) {
        this->books[bookID].available = true;
        this->books[bookID].borrower = "";
    }

    return "Book successfully checked in.";
}