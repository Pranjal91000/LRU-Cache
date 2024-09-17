#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

// Structure to hold book information
struct Book {
    int id;
    std::string title;
    std::string author;
    std::string isbn;
    int year;
};

// LRU Cache class
class LRUCache {
private:
    int capacity;
    std::list<int> cache; // Stores book IDs for LRU
    std::unordered_map<int, std::pair<Book, std::list<int>::iterator>> cacheMap; // Maps book ID to book details and iterator in LRU list

public:
    LRUCache(int cap) : capacity(cap) {}

    Book* get(int id) {
        // Check if book is in cache
        if (cacheMap.find(id) == cacheMap.end()) {
            return nullptr;
        }
        // Move the accessed book ID to the front of the LRU list
        cache.erase(cacheMap[id].second);
        cache.push_front(id);
        cacheMap[id].second = cache.begin();
        return &cacheMap[id].first;
    }

    void put(int id, const Book &book) {
        if (cacheMap.find(id) != cacheMap.end()) {
            // If already in cache, update and move to front
            cache.erase(cacheMap[id].second);
        } else if (cache.size() == capacity) {
            // If cache is full, remove the least recently used book (back of the list)
            int lru = cache.back();
            cache.pop_back();
            cacheMap.erase(lru);
        }
        // Add new book to the cache
        cache.push_front(id);
        cacheMap[id] = {book, cache.begin()};
    }

    void remove(int id) {
        if (cacheMap.find(id) != cacheMap.end()) {
            cache.erase(cacheMap[id].second);
            cacheMap.erase(id);
        }
    }

    // Display cache for debugging
    void displayCache() {
        std::cout << "Cache content (Book IDs): ";
        for (auto it : cache) {
            std::cout << it << " ";
        }
        std::cout << std::endl;
    }
};

// LFU Cache refresh mechanism
void refreshCache(LRUCache &cache, std::unordered_map<int, int> &frequencyMap, int refreshDuration) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(refreshDuration));

        // Remove least frequently used items based on frequencyMap
        if (!frequencyMap.empty()) {
            int minFreqID = frequencyMap.begin()->first;
            int minFreq = frequencyMap.begin()->second;

            for (const auto &pair : frequencyMap) {
                if (pair.second < minFreq) {
                    minFreqID = pair.first;
                    minFreq = pair.second;
                }
            }

            // Remove the least frequently used item from the cache
            std::cout << "Refreshing cache: Removing least frequently used book ID " << minFreqID << std::endl;
            frequencyMap.erase(minFreqID);
            cache.remove(minFreqID);
        }
    }
}

// Function to read books from CSV
std::unordered_map<int, Book> loadBooksFromCSV(const std::string &filename) {
    std::unordered_map<int, Book> bookData;
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string id, title, author, isbn, year;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, isbn, ',');
        getline(ss, year, ',');

        Book book = {std::stoi(id), title, author, isbn, std::stoi(year)};
        bookData[book.id] = book;
    }

    return bookData;
}

// Function to write books to CSV
void writeBooksToCSV(const std::string &filename, const std::unordered_map<int, Book> &bookData) {
    std::ofstream file(filename);
    file << "ID,Title,Author,ISBN,Year\n";
    for (const auto &pair : bookData) {
        const Book &book = pair.second;
        file << book.id << "," << book.title << "," << book.author << "," << book.isbn << "," << book.year << "\n";
    }
}

// Function to add a book to the library
void addBook(std::unordered_map<int, Book> &bookData, const std::string &filename, LRUCache &cache) {
    int id;
    std::string title, author, isbn;
    int year;
    
    std::cout << "Enter book ID: ";
    std::cin >> id;
    std::cin.ignore();
    std::cout << "Enter book title: ";
    getline(std::cin, title);
    std::cout << "Enter book author: ";
    getline(std::cin, author);
    std::cout << "Enter book ISBN: ";
    getline(std::cin, isbn);
    std::cout << "Enter book year: ";
    std::cin >> year;

    Book newBook = {id, title, author, isbn, year};
    bookData[id] = newBook;

    // Add to cache
    cache.put(id, newBook);

    // Write to CSV
    writeBooksToCSV(filename, bookData);
    std::cout << "Book added successfully." << std::endl;
}

// Function to remove a book from the library
void removeBook(std::unordered_map<int, Book> &bookData, const std::string &filename, LRUCache &cache, std::unordered_map<int, int> &frequencyMap) {
    int id;
    std::cout << "Enter book ID to remove: ";
    std::cin >> id;

    if (bookData.find(id) != bookData.end()) {
        // Remove from data and cache
        bookData.erase(id);
        cache.remove(id);
        frequencyMap.erase(id);

        // Write to CSV
        writeBooksToCSV(filename, bookData);
        std::cout << "Book removed successfully." << std::endl;
    } else {
        std::cout << "Book ID not found." << std::endl;
    }
}

int main() {
    const int cacheCapacity = 3; // Capacity of the cache
    const int refreshDuration = 10; // Time duration for cache refresh (in seconds)
    const std::string filename = "books.csv";

    // Load books from CSV file
    std::unordered_map<int, Book> bookData = loadBooksFromCSV(filename);

    // Initialize cache and frequency map
    LRUCache cache(cacheCapacity);
    std::unordered_map<int, int> frequencyMap;

    // Start the LFU refresh thread
    std::thread refreshThread(refreshCache, std::ref(cache), std::ref(frequencyMap), refreshDuration);

    int choice;
    while (true) {
        std::cout << "\nLibrary Menu:\n";
        std::cout << "1. Look up book\n";
        std::cout << "2. Add book\n";
        std::cout << "3. Remove book\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 4) break;

        switch (choice) {
            case 1: {
                int id;
                std::cout << "Enter book ID to look up: ";
                std::cin >> id;

                Book *book = cache.get(id);
                if (book) {
                    std::cout << "Book found in cache: " << book->title << " by " << book->author << std::endl;
                    frequencyMap[id]++;
                } else {
                    std::cout << "Book not in cache. Fetching from file..." << std::endl;
                    if (bookData.find(id) != bookData.end()) {
                        cache.put(id, bookData[id]);
                        frequencyMap[id] = 1;
                        std::cout << "Book: " << bookData[id].title << " by " << bookData[id].author << " added to cache." << std::endl;
                    } else {
                        std::cout << "Book ID not found in library." << std::endl;
                    }
                }

                cache.displayCache();
                break;
            }
            case 2:
                addBook(bookData, filename, cache);
                break;
            case 3:
                removeBook(bookData, filename, cache, frequencyMap);
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                break;
        }
    }

    // Join the refresh thread before exiting
    refreshThread.detach();

    return 0;
}
