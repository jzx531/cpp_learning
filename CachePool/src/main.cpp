#include <iostream>
#include <memory>
#include "Cache.h"
#include "LRUCache.h"
#include "RRCache.h"
#include "LFUCache.h"
#include "FIFOCache.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing LRUCache via Cache<int>* pointer\n";
    std::cout << "========================================\n";

    // --- Test LRUCache ---
    Cache<int>* lru = new LRUCache<int>(3);  // capacity = 3

    lru->put(1, 10);
    lru->put(2, 20);
    lru->put(3, 30);

    std::cout << "After put(1,10), put(2,20), put(3,30):\n";
    std::cout << "get(1) = " << lru->get(1) << " (expect 10)\n";
    std::cout << "get(2) = " << lru->get(2) << " (expect 20)\n";
    std::cout << "get(3) = " << lru->get(3) << " (expect 30)\n";

    // Access key=1 to make it recently used
    lru->get(1);

    // Insert new key → should evict key=2 (least recently used)
    lru->put(4, 40);
    std::cout << "\nAfter put(4,40) [capacity=3, LRU eviction]:\n";
    std::cout << "get(1) = " << lru->get(1) << " (should be 10)\n";
    std::cout << "get(2) = " << lru->get(2) << " (should be -1, evicted)\n";
    std::cout << "get(3) = " << lru->get(3) << " (should be 30)\n";
    std::cout << "get(4) = " << lru->get(4) << " (should be 40)\n";

    delete lru;

    std::cout << "\n========================================\n";
    std::cout << "Testing RRCache via Cache<int>* pointer\n";
    std::cout << "========================================\n";

    // --- Test RRCache ---
    Cache<int>* rr = new RRCache<int>(3);  // capacity = 3

    rr->put(1, 10);
    rr->put(2, 20);
    rr->put(3, 30);

    std::cout << "After put(1,10), put(2,20), put(3,30):\n";
    std::cout << "get(1) = " << rr->get(1) << "\n";
    std::cout << "get(2) = " << rr->get(2) << "\n";
    std::cout << "get(3) = " << rr->get(3) << "\n";

    // Insert 4th element → randomly evict one of {1,2,3}
    rr->put(4, 40);
    std::cout << "\nAfter put(4,40) [random eviction]:\n";
    int v1 = rr->get(1);
    int v2 = rr->get(2);
    int v3 = rr->get(3);
    int v4 = rr->get(4);

    std::cout << "get(1) = " << v1 << "\n";
    std::cout << "get(2) = " << v2 << "\n";
    std::cout << "get(3) = " << v3 << "\n";
    std::cout << "get(4) = " << v4 << "\n";

    // Count how many are valid (should be exactly 3)
    int validCount = (v1 != -1) + (v2 != -1) + (v3 != -1) + (v4 != -1);
    std::cout << "Valid entries: " << validCount << " (must be 3)\n";

    // Update existing key
    rr->put(4, 400);
    std::cout << "\nAfter updating key=4 to 400:\n";
    std::cout << "get(4) = " << rr->get(4) << " (expect 400)\n";

    // Insert another → evict one more randomly
    rr->put(5, 500);
    std::cout << "\nAfter put(5,500):\n";
    std::cout << "Final state:\n";
    for (int k = 1; k <= 5; ++k) {
        std::cout << "key " << k << " -> " << rr->get(k) << "\n";
    }

    // Final count check
    int finalValid = 0;
    for (int k = 1; k <= 5; ++k) {
        if (rr->get(k) != -1) finalValid++;
    }
    std::cout << "Final valid entries: " << finalValid << " (must be 3)\n";

    delete rr;

    Cache<int>* cache = new LFUCache<int>(2);

    // Step 1: put(1, 1)
    cache->put(1, 1);
    std::cout << "put(1, 1)\n";

    // Step 2: put(2, 2)
    cache->put(2, 2);
    std::cout << "put(2, 2)\n";

    // Step 3: get(1) -> freq[1] = 1 → becomes 2
    std::cout << "get(1) = " << cache->get(1) << " (expect 1)\n";

    // Step 4: put(3, 3) → should evict key=2 (freq=1 < freq=2 of key=1)
    cache->put(3, 3);
    std::cout << "put(3, 3) [evict least frequent]\n";

    // Check results
    std::cout << "get(1) = " << cache->get(1) << " (should be 1)\n";
    std::cout << "get(2) = " << cache->get(2) << " (should be -1, evicted)\n";
    std::cout << "get(3) = " << cache->get(3) << " (should be 3)\n";

    // Step 5: get(3) → freq[3] becomes 2
    cache->get(3);
    // Step 6: put(4, 4) → both key=1 and key=3 have freq=2, evict one (say key=1)
    cache->put(4, 4);
    std::cout << "\nAfter get(3) and put(4,4):\n";
    std::cout << "get(1) = " << cache->get(1) << " (may be -1)\n";
    std::cout << "get(3) = " << cache->get(3) << "\n";
    std::cout << "get(4) = " << cache->get(4) << "\n";
    // delete cache;

    cache = new FIFOCache<int>(2);

    // Step 1: put(1, 1)
    cache->put(1, 1);
    std::cout << "put(1, 1)\n";

    // Step 2: put(2, 2)
    cache->put(2, 2);
    std::cout << "put(2, 2)\n";

    // Step 3: get(1) -> freq[1] = 1 → becomes 2
    std::cout << "get(1) = " << cache->get(1) << " (expect 1)\n";

    // Step 4: put(3, 3) → should evict key=2 (freq=1 < freq=2 of key=1)
    cache->put(3, 3);
    std::cout << "put(3, 3) [evict least frequent]\n";

    // Check results
    std::cout << "get(1) = " << cache->get(1) << " (should be -1)\n";
    std::cout << "get(2) = " << cache->get(2) << " (should be 2)\n";
    std::cout << "get(3) = " << cache->get(3) << " (should be 3)\n";

    // Step 5: get(3) → freq[3] becomes 2
    cache->get(3);
    // Step 6: put(4, 4) → both key=1 and key=3 have freq=2, evict one (say key=1)
    cache->put(4, 4);
    std::cout << "\nAfter get(3) and put(4,4):\n";
    std::cout << "get(2) = " << cache->get(2) << " (may be -1)\n";
    std::cout << "get(3) = " << cache->get(3) << "\n";
    std::cout << "get(4) = " << cache->get(4) << "\n";
    delete cache;

    std::cout << "All tests completed successfully!\n";
    return 0;
}