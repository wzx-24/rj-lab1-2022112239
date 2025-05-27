#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../include/lab1.h"
using namespace std;

void printPath(const vector<string>& path) {
    if (path.empty()) {
        cout << "(empty)";
    } else {
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i != path.size() - 1) cout << " -> ";
        }
    }
    cout << endl;
}

bool vecEqual(const vector<string>& a, const vector<string>& b) {
    return a.size() == b.size() && equal(a.begin(), a.end(), b.begin());
}

int main() {
    // 1. 起点或终点不存在
    buildGraph({"a", "b", "c"});
    auto [path1, len1] = shortestPath("x", "b");
    cout << "Test 1 path: "; printPath(path1);
    assert(path1.empty() && len1 == -1);

    // 2. 起点到终点有直接路径
    buildGraph({"a", "b", "c"});
    auto [path2, len2] = shortestPath("a", "b");
    cout << "Test 2 path: "; printPath(path2);
    assert(vecEqual(path2, vector<string>{"a", "b"}) && len2 == 1);

    // 3. 起点到终点不可达
    buildGraph({"a", "b", "c"}); // 没有d
    auto [path3, len3] = shortestPath("a", "d");
    cout << "Test 3 path: "; printPath(path3);
    assert(path3.empty() && len3 == -1);
    
    // 4. 路径中有多条边，需多次松弛
    buildGraph({"a", "b", "c", "d"});
    auto [path4, len4] = shortestPath("a", "d");
    cout << "Test 4 path: "; printPath(path4);
    assert(vecEqual(path4, vector<string>{"a", "b", "c", "d"}) && len4 == 3);

    // 5. 路径中有环但不影响最短路径
    vector<string> words5 = {"a", "b", "c", "a", "d"};
    buildGraph(words5);
    auto [path5, len5] = shortestPath("a", "d");
    cout << "Test 5 path: "; printPath(path5);
    assert(vecEqual(path5, vector<string>{"a", "d"}) && len5 == 1);

    cout << "All shortestPath tests passed.\n";
    return 0;
}