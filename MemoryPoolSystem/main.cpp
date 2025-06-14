#include <iostream>
#include <vector>
#include "MemoryPool.hpp"

int main() {
    size_t defaultBlockSize = 64;
    size_t initialBlockCount = 5;
    MemoryPool pool(defaultBlockSize, initialBlockCount);
    std::vector<void*> allocated;

    int choice;
    do {
        std::cout << "\n==== Memory Pool Menu ====" << std::endl;
        std::cout << "1. Allocate memory" << std::endl;
        std::cout << "2. Release memory" << std::endl;
        std::cout << "3. Show memory status" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            size_t size;
            std::cout << "Enter size to allocate: ";
            std::cin >> size;
            void* ptr = pool.allocate(size);
            if (ptr) {
                allocated.push_back(ptr);
                std::cout << "Allocated " << size << " bytes." << std::endl;
            }
            else {
                std::cout << "Allocation failed." << std::endl;
            }
            break;
        }
        case 2: {
            if (allocated.empty()) {
                std::cout << "No allocations to release." << std::endl;
                break;
            }
            void* ptr = allocated.back();
            allocated.pop_back();
            if (pool.deallocate(ptr)) {
                std::cout << "Memory deallocated.\n";
            }
            break;
        }
        case 3: {
            std::cout << "\n--- Memory Pool Status ---" << std::endl;
            std::cout << "Total Pool Size: " << pool.totalPoolSize() << " bytes" << std::endl;
            std::cout << "Used Memory: " << pool.usedMemorySize() << " bytes" << std::endl;
            std::cout << "Free Memory: " << pool.freeMemorySize() << " bytes" << std::endl;
            std::cout << "Available Blocks: " << pool.available() << std::endl;
            break;
        }
        case 0:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
        }

    } while (choice != 0);

    for (auto ptr : allocated) {
        pool.deallocate(ptr);
    }

    return 0;
}
