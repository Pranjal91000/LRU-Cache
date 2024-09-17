## **Library Book Lookup System with LRU Cache**

This application is designed to manage and optimize library book lookups by implementing Least Recently Used (LRU) and Least Frequently Used (LFU) cache mechanisms. It interacts with a CSV file containing book data and provides an efficient way to retrieve, add, and remove books while minimizing repeated disk I/O operations.

## **Overview of Features and Functionality**

#### **1. Core Features:**

- **Book Lookup with LRU Cache**
  - **Efficient Access**: Fast retrieval of frequently accessed book details using an in-memory LRU cache.
  - **LRU Policy**: Books are ordered by recent access; least recently used books are evicted when the cache is full.

- **Least Frequently Used (LFU) Cache Refresh**
  - **Periodic Refresh**: A background thread periodically removes the least frequently accessed books from the cache after a set interval (e.g., 10 seconds).

- **CSV File Storage**
  - **Persistent Data**: Books are stored in a CSV file, ensuring data is retained across application restarts.
  - **Real-time Updates**: The file is updated when books are added or removed.

#### **2. Add and Remove Books:**

- **Add Book**
  - **User Input**: Prompts for book details (ID, title, author, ISBN, year).
  - **Data Management**: Adds the new book to both the CSV file and the cache.

- **Remove Book**
  - **User Input**: Prompts for the book ID to remove.
  - **Data Management**: Removes the book from both the CSV file and the cache.

#### **3. Cache Management Features:**

- **LRU Cache**
  - **Capacity-based**: Limits the number of books stored; evicts least recently used books as needed.
  - **Efficient Lookups**: Reduces time spent fetching book details from the CSV file.

- **LFU Cache Refresh**
  - **Automatic Eviction**: Removes least frequently accessed books to keep the cache optimal.

#### **4. User Interaction Features:**

- **Interactive Menu**
  - **Options**: Allows users to look up, add, or remove books, or exit the application.
  - **Feedback**: Provides real-time updates on book status and cache content.

- **Real-time Feedback**
  - **Information**: Notifies users of cache hits, misses, and the status of book additions or removals.

#### **5. Multithreading:**

- **Background LFU Refresh Thread**
  - **Function**: Continuously manages cache refresh based on access frequency without interrupting user interactions.
  - **Operation**: Runs in parallel with the main thread, performing periodic cache clean-up tasks.

- **Main Thread**
  - **Function**: Handles user input for book operations (lookup, add, remove).
  - **Operation**: Interacts with the cache and CSV file to perform tasks requested by the user.

This comprehensive approach ensures the application is efficient, scalable, and user-friendly, managing book data effectively with optimal performance through caching and multi-threading.
