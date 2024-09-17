#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>

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

int main() {
    const int cacheCapacity = 3; // Capacity of the cache
    const int refreshDuration = 10; // Time duration for cache refresh (in seconds)

    // Load books from CSV file
    std::unordered_map<int, Book> bookData = loadBooksFromCSV("books.csv");

    // Initialize cache and frequency map
    LRUCache cache(cacheCapacity);
    std::unordered_map<int, int> frequencyMap;

    // Start the LFU refresh thread
    std::thread refreshThread(refreshCache, std::ref(cache), std::ref(frequencyMap), refreshDuration);

    while (true) {
        int id;
        std::cout << "Enter book ID to look up (or -1 to exit): ";
        std::cin >> id;

        if (id == -1) break;

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

        cache.displayCache(); // Display current cache content for debugging
    }

    // Join the refresh thread before exiting
    refreshThread.detach();

    return 0;
}
