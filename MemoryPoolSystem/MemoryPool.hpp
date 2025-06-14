#pragma once
#include <cstddef>
#include <vector>
#include <unordered_map>

class MemoryPool {
private:
    struct Block {
        Block* next;
    };

    std::vector<char*> chunks;    // Stores all memory chunks
    Block* freeList;              // Free block list
    size_t blockSize;
    size_t totalBlocks;
    size_t usedBlocks;
    size_t expansionFactor;      // New: tracks how much to expand by

    std::unordered_map<void*, size_t> allocationMap; // Track blocks per allocation

    void expandPool(size_t minBlockSize); // Auto expand pool

public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    void* allocate(size_t size);           // Allocate memory (any size)
    bool deallocate(void* ptr);            // Deallocate memory (returns true if successful)
    size_t available() const;              // Memory status
    size_t totalPoolSize() const;          // Total allocated pool size in bytes
    size_t usedMemorySize() const;         // Used memory in bytes
    size_t freeMemorySize() const;         // Free memory in bytes
};

