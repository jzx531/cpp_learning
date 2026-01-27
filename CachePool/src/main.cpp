#include <iostream>
#include "LRUCache.h"  // 假设你的头文件和实现都在 Cache.h（或分开的话需包含 .cpp）

// 注意：由于模板定义通常放在头文件中，若你的实现（.cpp）单独存在，
// 则需确保模板实例化或直接将实现写在头文件中。
// 这里假设所有代码都在 Cache.h 中（如你所示，实现已在头文件末尾）

int main() {
    LRUCache<int> lru(2);

    // 初始状态：空缓存
    std::cout << "get(1): " << lru.get(1) << " (expected: -1)" << std::endl;

    // put(1, 1)
    lru.put(1, 1);
    std::cout << "put(1,1)" << std::endl;

    // put(2, 2)
    lru.put(2, 2);
    std::cout << "put(2,2)" << std::endl;

    // get(1) -> 应该返回 1，并将 1 移到头部（最近使用）
    std::cout << "get(1): " << lru.get(1) << " (expected: 1)" << std::endl;

    // put(3, 3) -> 容量已满，应淘汰 key=2（因为 2 最久未用）
    lru.put(3, 3);
    std::cout << "put(3,3) [capacity=2, evict key=2]" << std::endl;

    // get(2) -> 应该返回 -1（已被淘汰）
    std::cout << "get(2): " << lru.get(2) << " (expected: -1)" << std::endl;

    // get(3) -> 应该返回 3
    std::cout << "get(3): " << lru.get(3) << " (expected: 3)" << std::endl;

    // 再 put(4,4) -> 淘汰 key=1（现在 1 是最久未用）
    lru.put(4, 4);
    std::cout << "put(4,4) [evict key=1]" << std::endl;

    // get(1) -> -1
    std::cout << "get(1): " << lru.get(1) << " (expected: -1)" << std::endl;
    // get(3) -> 3
    std::cout << "get(3): " << lru.get(3) << " (expected: 3)" << std::endl;
    // get(4) -> 4
    std::cout << "get(4): " << lru.get(4) << " (expected: 4)" << std::endl;

    return 0;
}