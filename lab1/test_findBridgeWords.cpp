#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../include/lab1.h"
using namespace std;

void printBridges(const vector<string>& bridges) {
    if (bridges.empty()) {
        cout << "(empty)";
    } else {
        for (size_t i = 0; i < bridges.size(); ++i) {
            cout << bridges[i];
            if (i != bridges.size() - 1) cout << ", ";
        }
    }
    cout << endl;
}

int main() {
    // 构造简单图: a->b->c->d->b->e->c
    vector<string> words = {"a", "b", "c", "d", "b", "e", "c"};
    buildGraph(words);

    // 黑盒测试1：正常存在桥接词
    auto bridges1 = findBridgeWords("a", "c");
    cout << "Test 1 bridges: ";
    printBridges(bridges1);
    assert(bridges1.size() == 1 && bridges1[0] == "b");
    cout << "Test 1 passed.\n";

    // 黑盒测试2：无桥接词（a->b, b没有直接到d）
    auto bridges2 = findBridgeWords("a", "d");
    cout << "Test 2 bridges: ";
    printBridges(bridges2);
    assert(bridges2.empty());
    cout << "Test 2 passed.\n";

    // 黑盒测试3-1：起点不存在，终点存在
    auto bridges3_1 = findBridgeWords("x", "b");
    cout << "Test 3-1 bridges: ";
    printBridges(bridges3_1);
    assert(bridges3_1.empty());
    cout << "Test 3-1 passed.\n";

    // 黑盒测试3-2：起点存在，终点不存在
    auto bridges3_2 = findBridgeWords("a", "y");
    cout << "Test 3-2 bridges: ";
    printBridges(bridges3_2);
    assert(bridges3_2.empty());
    cout << "Test 3-2 passed.\n";

    // 黑盒测试3-3：起点和终点都不存在
    auto bridges3_3 = findBridgeWords("x", "y");
    cout << "Test 3-3 bridges: ";
    printBridges(bridges3_3);
    assert(bridges3_3.empty());
    cout << "Test 3-3 passed.\n";

    // 黑盒测试4：多个桥接词
    // 构造 a->b->c, a->d->c
    vector<string> words2 = {"a", "b", "c", "a", "d", "c"};
    buildGraph(words2);
    auto bridges4 = findBridgeWords("a", "c");
    cout << "Test 4 bridges: ";
    printBridges(bridges4);
    assert(bridges4.size() == 2);
    cout << "Test 4 passed.\n";

    cout << "All tests passed.\n";
    return 0;
}