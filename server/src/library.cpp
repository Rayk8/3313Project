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
        b.image = info["image"];
        books[bookID] = b;
    }
}

std::string Library::getCatalog() {
    std::lock_guard<std::mutex> lock(libraryMutex);
    std::string html = "<div style='display: flex; flex-wrap: wrap; gap: 1rem;'>";

    for (const auto& pair : books) {
        const Book& book = pair.second;

        html += "<div style='border: 1px solid #ccc; padding: 1rem; width: 250px; border-radius: 8px; box-shadow: 0 2px 6px rgba(0,0,0,0.1);'>";
        html += "<img src='" + book.image + "' alt='Cover' style='width:100%; height:180px; object-fit:contain; margin-bottom:10px;'>";
html += "<h3>" + book.title + "</h3>";
html += "<p><strong>ID:</strong> " + book.bookID + "</p>";

        if (book.available) {
            html += "<button  class='borrow-btn' onclick=\"checkoutBook('" + book.bookID + "')\">Borrow</button>";
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
                << "<label for='rating_" << id << "'>Rate this book: </label>"
                << "<select id='rating_" << id << "'>"
                << "<option value=''>--</option>"
                << "<option value='1'>1</option>"
                << "<option value='2'>2</option>"
                << "<option value='3'>3</option>"
                << "<option value='4'>4</option>"
                << "<option value='5'>5</option>"
                << "</select> "
                << "<button onclick=\"submitRating('" << id << "')\">Submit Rating</button><br><br>"
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
    timestamp << std::put_time(std::localtime(&now_time), "%Y-%m-%d %I:%M %p");

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
    timestamp << std::put_time(std::localtime(&now_time), "%Y-%m-%d %I:%M %p");

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

std::string Library::rateBook(const std::string& username, const std::string& bookID, int rating) {
    std::lock_guard<std::mutex> lock(userMutex);
    json ratings = loadJson("Ratings.json");

    ratings[username][bookID] = rating;
    saveJson("Ratings.json", ratings);

    return "Thank you for rating!";
}

std::string Library::getRatingsHtml(const std::string& username) {
    std::lock_guard<std::mutex> lock(libraryMutex);

    json ratings = loadJson("Ratings.json");
    json booksData = loadJson("books.json");

    if (!ratings.contains(username)) return "<p>You haven't rated any books yet.</p>";

    std::ostringstream html;
    html << "<div style='display: flex; flex-wrap: wrap; gap: 1rem;'>";

    for (auto& [bookID, rating] : ratings[username].items()) {
        if (!booksData.contains(bookID)) continue;

        const auto& book = booksData[bookID];
        std::string title = book["title"];
        std::string image = book["image"];

        html << "<div style='border: 1px solid #ccc; padding: 1rem; width: 250px; border-radius: 8px; box-shadow: 0 2px 6px rgba(0,0,0,0.1);'>";
        html << "<img src='" << image << "' alt='Book cover' style='width:100%; height:180px; object-fit:contain; margin-bottom:10px;'>";
        html << "<h3>" << title << "</h3>";
        html << "<p><strong>Your Rating:</strong> " << rating << " / 5</p>";
        html << "</div>";
    }

    html << "</div>";
    return html.str();
}

std::string Library::getHistoryHtml(const std::string& username) {
    std::lock_guard<std::mutex> lock(libraryMutex);
    json transactions = loadJson("Transactions.json");

    if (!transactions.contains(username)) {
        return "<p>You have no book history yet.</p>";
    }

    std::ostringstream html;
    html << "<div style='display: flex; flex-direction: column; gap: 1rem;'>";

    for (const auto& entry : transactions[username]) {
        if (entry["action"] != "return") continue;

        std::string rawTime = entry["timestamp"];
        std::tm tm = {};
        std::istringstream ss(rawTime);
        ss >> std::get_time(&tm, "%Y-%m-%d %I:%M %p");

        if (ss.fail()) {
            std::cerr << "Failed to parse timestamp: " << rawTime << std::endl;
        }

        char formatted[100];
        std::strftime(formatted, sizeof(formatted), "%B %d, %Y at %I:%M %p", &tm);

        html << "<div class='book-container'>";
        html << "<h3>" << entry["title"].get<std::string>() << "</h3>";
        html << "<p><strong>Returned at:</strong> " << formatted << "</p>";
        html << "</div>";
    }

    html << "</div>";
    return html.str();
}

