#include "MemoryPool.hpp"
#include <new>
#include <algorithm>
#include <iostream>

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : blockSize((blockSize + sizeof(void*) - 1) & ~(sizeof(void*) - 1)),
    totalBlocks(blockCount),
    usedBlocks(0),
    freeList(nullptr),
    expansionFactor(blockCount) // base expansion unit = initial pool size
{
    expandPool(blockSize); // Initial allocation
}

MemoryPool::~MemoryPool() {
    for (auto& chunk : chunks) {
        delete[] chunk;
    }
    chunks.clear();
    allocationMap.clear();
}

void* MemoryPool::allocate(size_t size) {
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    size_t blocksNeeded = (size + blockSize - 1) / blockSize;

    while (available() < blocksNeeded) {
        expandPool(blockSize);
        if (!freeList) return nullptr;
    }

    Block* result = freeList;
    Block* current = result;
    for (size_t i = 0; i < blocksNeeded - 1; ++i) {
        current = current->next;
    }
    Block* nextFree = current->next;
    current->next = nullptr;

    freeList = nextFree;
    usedBlocks += blocksNeeded;
    allocationMap[result] = blocksNeeded;
    std::cout << "[Info] Requested " << size << " bytes → Allocated " << blocksNeeded << " blocks (" << blocksNeeded * blockSize << " bytes)\n";
    return static_cast<void*>(result);
}

bool MemoryPool::deallocate(void* ptr) {
    if (!ptr) return false;

    auto it = allocationMap.find(ptr);
    if (it == allocationMap.end()) {
        std::cerr << "[Warning] Attempted to free untracked or already freed pointer.\n";
        return false;
    }

    size_t blocksToFree = it->second;
    Block* start = static_cast<Block*>(ptr);
    Block* current = start;

    for (size_t i = 1; i < blocksToFree; ++i) {
        Block* next = reinterpret_cast<Block*>(reinterpret_cast<char*>(current) + blockSize);
        current->next = next;
        current = next;
    }

    current->next = freeList;
    freeList = start;
    usedBlocks -= blocksToFree;
    allocationMap.erase(it);
    return true;
}

size_t MemoryPool::available() const {
    return totalBlocks - usedBlocks;
}

size_t MemoryPool::totalPoolSize() const {
    return totalBlocks * blockSize;
}

size_t MemoryPool::usedMemorySize() const {
    return usedBlocks * blockSize;
}

size_t MemoryPool::freeMemorySize() const {
    return available() * blockSize;
}

void MemoryPool::expandPool(size_t minBlockSize) {
    // Expand by 1.5x the previous chunk size
    size_t newBlocks = (expansionFactor * 3) / 2;
    expansionFactor = newBlocks; // update next expansion unit

    char* newPool = new (std::nothrow) char[blockSize * newBlocks];
    if (!newPool) return;

    chunks.push_back(newPool);
    for (size_t i = 0; i < newBlocks; ++i) {
        Block* block = reinterpret_cast<Block*>(newPool + i * blockSize);
        block->next = freeList;
        freeList = block;
    }
    totalBlocks += newBlocks;

    std::cout << "[Pool Expanded] Added " << newBlocks << " blocks. Total blocks: " << totalBlocks << ", Total Pool Size: " << totalPoolSize() << " bytes\n";
    std::cout << "[Status] Used: " << usedMemorySize() << " bytes, Free: " << freeMemorySize() << " bytes\n";
}

