#include <iostream>
#include <unordered_map>

class InMemoryDB {
private:
    std::unordered_map<std::string, int> mainDB;
    std::unordered_map<std::string, int> transactionDB;
    bool isTransactionActive;

public:
    InMemoryDB() : isTransactionActive(false) {}

    std::optional<int> get(const std::string& key) {
        auto it = mainDB.find(key);
        if (it != mainDB.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void put(const std::string& key, int val) {
        if (!isTransactionActive) {
            throw std::runtime_error("No transaction in progress");
        }
        transactionDB[key] = val;
    }

    void begin_transaction() {
        if (isTransactionActive) {
            throw std::runtime_error("Transaction already in progress");
        }
        isTransactionActive = true;
        transactionDB.clear();
    }

    void commit() {
        if (!isTransactionActive) {
            throw std::runtime_error("No transaction in progress");
        }

        for (const auto& pair : transactionDB) {
            mainDB[pair.first] = pair.second;
        }
        transactionDB.clear();
        isTransactionActive = false;
    }

    void rollback() {
        if (!isTransactionActive) {
            throw std::runtime_error("No transaction in progress");
        }
        transactionDB.clear();
        isTransactionActive = false;
    }
};

int main() {
    InMemoryDB inmemoryDB;

    // Test 1: Get key that doesn't exist
    std::cout << "Test 1: Get key that doesn't exist (should be null): ";
    auto result = inmemoryDB.get("A");
    std::cout << (result.has_value() ? std::to_string(result.value()) : "null") << std::endl;

    // Test 2: Put without transaction
    std::cout << "\nTest 2: Put without transaction (should throw an error): ";
    try {
        inmemoryDB.put("A", 5);
    } catch (const std::runtime_error& error) {
        std::cout << "Error caught: " << error.what() << std::endl;
    }

    // Test 3: Begin transaction and put 'A' = 5
    std::cout << "\nTest 3: Begin transaction and put 'A' = 5";
    inmemoryDB.begin_transaction();
    inmemoryDB.put("A", 5);
    std::cout << "\nGet 'A' (should be null): ";
    result = inmemoryDB.get("A");
    std::cout << (result.has_value() ? std::to_string(result.value()) : "null") << std::endl;

    // Test 4: Update 'A' to 6 within transaction
    std::cout << "\nTest 4: Update 'A' to 6 within transaction";
    inmemoryDB.put("A", 6);
    inmemoryDB.commit();
    std::cout << "\nGet 'A' after commit (should be 6): ";
    result = inmemoryDB.get("A");
    std::cout << (result.has_value() ? std::to_string(result.value()) : "null") << std::endl;

    // Test 5: Commit without transaction
    std::cout << "\nTest 5: Commit without transaction (should throw an error): ";
    try {
        inmemoryDB.commit();
    } catch (const std::runtime_error& error) {
        std::cout << "Error caught: " << error.what() << std::endl;
    }

    // Test 6: Rollback without transaction
    std::cout << "\nTest 6: Rollback without transaction (should throw an error): ";
    try {
        inmemoryDB.rollback();
    } catch (const std::runtime_error& error) {
        std::cout << "Error caught: " << error.what() << std::endl;
    }

    // Test 7: Transaction rollback
    std::cout << "\nTest 7: Get 'B' (should be null): ";
    result = inmemoryDB.get("B");
    std::cout << (result.has_value() ? std::to_string(result.value()) : "null") << std::endl;

    std::cout << "Begin transaction and put 'B' = 10" << std::endl;
    inmemoryDB.begin_transaction();
    inmemoryDB.put("B", 10);
    inmemoryDB.rollback();

    std::cout << "Get 'B' after rollback (should be null): ";
    result = inmemoryDB.get("B");
    std::cout << (result.has_value() ? std::to_string(result.value()) : "null") << std::endl;

    return 0;
}