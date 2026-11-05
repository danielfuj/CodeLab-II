#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Structure to represent individual book records
struct Book {
    string id;
    string title;
    string author;
    int pages = 0;
    bool available = true;
};

class BookManager {
private:
    vector<Book> books;
    const string dataFile = "bookData.txt";

    // Helper function to read text input from the user
    string readText(const string& prompt) const {
        cout << prompt;
        string value;
        getline(cin, value);
        return value;
    }

    // Helper function to safely read and validate integer inputs within a range
    int readInteger(const string& prompt, int minimum, int maximum) const {
        while (true) {
            const string input = readText(prompt);
            try {
                size_t used = 0;
                int value = stoi(input, &used);
                if (used == input.size() && value >= minimum && value <= maximum)
                    return value;
            } catch (...) {
            }
            cout << "Enter a valid number between " << minimum << " and " << maximum << ".\n";
        }
    }

    // Locates a book by its unique ID and returns its index if found
    optional<size_t> locate(const string& id) const {
        for (size_t i = 0; i < books.size(); ++i)
            if (books[i].id == id) return i;
        return nullopt;
    }

    // Loads book records from file paths, or initializes default records if file is missing
    void load() {
        const array<string, 3> paths = {
            "Book Manager/bookData.txt",
            "S1-Programming-Skills-Portfolio/Book Manager/bookData.txt",
            "bookData.txt"
        };

        for (const auto& path : paths) {
            ifstream input(path);
            if (!input) continue;

            string line;
            while (getline(input, line)) {
                stringstream row(line);
                Book book;
                string pages;
                string status;

                if (!getline(row, book.title, ',') ||
                    !getline(row, book.author, ',') ||
                    !getline(row, pages, ',') ||
                    !getline(row, book.id, ',') ||
                    !getline(row, status))
                    continue;

                try {
                    book.pages = stoi(pages);
                    book.available = status == "true";
                    if (!book.id.empty() && book.pages > 0 && !locate(book.id))
                        books.push_back(book);
                } catch (...) {
                }
            }

            if (!books.empty()) return;
        }

        // Default initial catalogue if no data file is found
        books = {
            {"001", "The Great Gatsby", "F. Scott Fitzgerald", 180, true},
            {"002", "1984", "George Orwell", 328, false},
            {"003", "To Kill a Mockingbird", "Harper Lee", 281, true},
            {"004", "Pride and Prejudice", "Jane Austen", 279, false},
            {"005", "The Catcher in the Rye", "J.D. Salinger", 214, true},
            {"006", "Moby-Dick", "Herman Melville", 635, false},
            {"007", "War and Peace", "Leo Tolstoy", 1225, true},
            {"008", "The Hobbit", "J.R.R. Tolkien", 310, false},
            {"009", "Crime and Punishment", "Fyodor Dostoevsky", 671, true},
            {"010", "Brave New World", "Aldous Huxley", 268, false}
        };
    }

    // Saves current book records back to the data file
    void save() const {
        ofstream output(dataFile);
        if (!output) {
            cout << "Unable to save the catalogue.\n";
            return;
        }

        for (const auto& book : books) {
            output << book.title << ',' << book.author << ',' << book.pages << ','
                   << book.id << ',' << (book.available ? "true" : "false") << '\n';
        }
    }

    // Displays a formatted table of a given collection of books
    void display(const vector<Book>& collection) const {
        if (collection.empty()) {
            cout << "No books found.\n";
            return;
        }

        cout << '\n' << left
             << setw(9) << "ID"
             << setw(32) << "TITLE"
             << setw(27) << "AUTHOR"
             << setw(9) << "PAGES"
             << "STATUS\n"
             << string(88, '-') << '\n';

        for (const auto& book : collection) {
            cout << left << setw(9) << book.id
                 << setw(32) << book.title.substr(0, 30)
                 << setw(27) << book.author.substr(0, 25)
                 << setw(9) << book.pages
                 << (book.available ? "Available" : "On loan") << '\n';
        }
    }

    // Searches for books matching a keyword in title, author, or ID
    void search() const {
        const string query = readText("Search by title, author or ID: ");
        vector<Book> results;

        for (const auto& book : books) {
            string content = book.title + " " + book.author + " " + book.id;
            string searchable = content;
            string term = query;

            transform(searchable.begin(), searchable.end(), searchable.begin(),
                [](unsigned char c) { return static_cast<char>(tolower(c)); });
            transform(term.begin(), term.end(), term.begin(),
                [](unsigned char c) { return static_cast<char>(tolower(c)); });

            if (searchable.find(term) != string::npos)
                results.push_back(book);
        }

        display(results);
        cout << results.size() << " result(s).\n";
    }

    // Adds a new book entry to the collection
    void add() {
        Book book;
        book.id = readText("Book ID: ");

        if (book.id.empty() || locate(book.id)) {
            cout << "A unique book ID is required.\n";
            return;
        }

        book.title = readText("Title: ");
        book.author = readText("Author: ");
        book.pages = readInteger("Number of pages: ", 1, 100000);

        if (book.title.empty() || book.author.empty()) {
            cout << "Title and author are required.\n";
            return;
        }

        book.available = true;
        books.push_back(book);
        save();
        cout << "Book added successfully.\n";
    }

    // Edits an existing book's details
    void edit() {
        const string id = readText("Enter the ID to edit: ");
        const auto index = locate(id);

        if (!index) {
            cout << "Book not found.\n";
            return;
        }

        Book& book = books[*index];
        cout << "Leave a field blank to keep its current value.\n";

        const string title = readText("Title [" + book.title + "]: ");
        const string author = readText("Author [" + book.author + "]: ");

        if (!title.empty()) book.title = title;
        if (!author.empty()) book.author = author;
        if (readText("Update page count? (y/n): ") == "y")
            book.pages = readInteger("New page count: ", 1, 100000);

        save();
        cout << "Book updated successfully.\n";
    }

    // Removes a book entry from the collection
    void remove() {
        const string id = readText("Enter the ID to remove: ");
        const auto index = locate(id);

        if (!index) {
            cout << "Book not found.\n";
            return;
        }

        cout << "Selected: " << books[*index].title << '\n';
        if (readText("Confirm deletion? (y/n): ") != "y") return;

        books.erase(books.begin() + static_cast<ptrdiff_t>(*index));
        save();
        cout << "Book removed.\n";
    }

    // Changes the availability status (borrow/return) of a book
    void changeStatus(bool returning) {
        const string id = readText("Enter book ID: ");
        const auto index = locate(id);

        if (!index) {
            cout << "Book not found.\n";
            return;
        }

        Book& book = books[*index];
        if (book.available == returning) {
            cout << (returning ? "The book is already available.\n" :
                                 "The book is already on loan.\n");
            return;
        }

        book.available = returning;
        save();
        cout << (returning ? "Book returned successfully.\n" :
                           "Book borrowed successfully.\n");
    }

    // Calculates and displays library statistics
    void statistics() const {
        const size_t available = count_if(books.begin(), books.end(),
            [](const Book& book) { return book.available; });
        long long totalPages = 0;

        for (const auto& book : books) totalPages += book.pages;

        cout << "\n========== LIBRARY STATISTICS ==========\n"
             << "Total books: " << books.size() << '\n'
             << "Available: " << available << '\n'
             << "On loan: " << books.size() - available << '\n'
             << "Combined page count: " << totalPages << '\n';

        if (!books.empty())
            cout << fixed << setprecision(1)
                 << "Availability rate: "
                 << (100.0 * available / books.size()) << "%\n";
    }

    // Sorts the catalogue based on user choice (title, author, or pages)
    void sortBooks() {
        cout << "1. Title\n2. Author\n3. Page count\n";
        const int choice = readInteger("Sort by: ", 1, 3);

        if (choice == 1) {
            sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.title < b.title;
            });
        } else if (choice == 2) {
            sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.author < b.author;
            });
        } else {
            sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.pages < b.pages;
            });
        }

        display(books);
    }

public:
    // Constructor initializes data loading
    BookManager() {
        load();
    }

    // Main application execution loop presenting the menu interface
    void run() {
        while (true) {
            cout << "\n========== BOOK MANAGER ==========\n"
                 << "1. Display catalogue\n"
                 << "2. Search books\n"
                 << "3. Add a book\n"
                 << "4. Edit a book\n"
                 << "5. Remove a book\n"
                 << "6. Borrow a book\n"
                 << "7. Return a book\n"
                 << "8. Library statistics\n"
                 << "9. Sort catalogue\n"
                 << "10. Exit\n";

            const int choice = readInteger("Select an option: ", 1, 10);

            switch (choice) {
                case 1: display(books); break;
                case 2: search(); break;
                case 3: add(); break;
                case 4: edit(); break;
                case 5: remove(); break;
                case 6: changeStatus(false); break;
                case 7: changeStatus(true); break;
                case 8: statistics(); break;
                case 9: sortBooks(); break;
                case 10: save(); return;
            }
        }
    }
};

int main() {
    BookManager application;
    application.run();
    return 0;
}