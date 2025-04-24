#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>
#include <random>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

// 全局数据结构//
unordered_map<string, unordered_map<string, int>> adjList;    // 邻接表
unordered_map<string, vector<string>> inEdges;                // 入边记录
unordered_set<string> nodes;                                  // 所有节点
mt19937 rng(time(nullptr));                                   // 随机数生成器

// 文件处理函数
vector<string> processFile(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    if (!file.is_open()) return words;

    string word;
    char c;
    while (file.get(c)) {
        if (isalpha(c)) {
            word += tolower(c);
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}

// 构建图结构
void buildGraph(const vector<string>& words) {
    nodes.clear();
    adjList.clear();
    inEdges.clear();

    for (size_t i = 0; i < words.size() - 1; ++i) {
        string from = words[i], to = words[i+1];
        nodes.insert(from);
        nodes.insert(to);
        adjList[from][to]++;
        inEdges[to].push_back(from);
    }
    if (!words.empty()) nodes.insert(words.back());
}

// 查找桥接词
vector<string> findBridgeWords(const string& w1, const string& w2) {
    string word1 = w1, word2 = w2;
    transform(word1.begin(), word1.end(), word1.begin(), ::tolower);
    transform(word2.begin(), word2.end(), word2.begin(), ::tolower);

    if (!nodes.count(word1) || !nodes.count(word2)) return {};
    vector<string> bridges;

    if (!adjList.count(word1)) return {};
    for (const auto& [word3, _] : adjList.at(word1)) {
        if (adjList.count(word3) && adjList[word3].count(word2)) {
            bridges.push_back(word3);
        }
    }
    return bridges;
}

// 插入桥接词到新文本
string insertBridgeWords(const string& input) {
    vector<string> new_words;
    stringstream ss(input);
    string word;
    
    // 过滤非字母字符并转换为小写
    while (ss >> word) {
        string filtered;
        for (char c : word) {
            if (isalpha(c)) filtered += tolower(c);
        }
        if (!filtered.empty()) new_words.push_back(filtered);
    }

    vector<string> result;
    for (size_t i = 0; i < new_words.size(); ++i) {
        result.push_back(new_words[i]);
        if (i < new_words.size() - 1) {
            auto bridges = findBridgeWords(new_words[i], new_words[i+1]);
            if (!bridges.empty()) {
                uniform_int_distribution<int> dist(0, bridges.size()-1);
                result.push_back(bridges[dist(rng)]);
            }
        }
    }

    // 重建输出字符串
    string output;
    for (size_t i = 0; i < result.size(); ++i) {
        output += result[i];
        if (i != result.size()-1) output += " ";
    }
    return output;
}

// 最短路径计算（Dijkstra算法）
pair<vector<string>, int> shortestPath(const string& start, const string& end) {
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> pq;

    // 初始化
    for (const auto& node : nodes) dist[node] = numeric_limits<int>::max();
    string s = start;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    string e = end;
    transform(e.begin(), e.end(), e.begin(), ::tolower);
    
    if (!nodes.count(s) || !nodes.count(e)) return {{}, -1};

    dist[s] = 0;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (u == e) break;
        if (d > dist[u]) continue;

        for (const auto& [v, w] : adjList[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    // 不可达
    if (dist[e] == numeric_limits<int>::max()) return {{}, -1};

    // 重建路径
    vector<string> path;
    for (string cur = e; cur != s; cur = prev[cur]) {
        path.push_back(cur);
        if (!prev.count(cur)) return {{}, -1};
    }
    path.push_back(s);
    reverse(path.begin(), path.end());
    return {path, dist[e]};
}

// PageRank计算
unordered_map<string, double> calculatePageRank(double d = 0.85, double threshold = 1e-6, int max_iter = 100) {
    unordered_map<string, double> pr_old, pr_new;
    const double init = 1.0 / nodes.size();
    const int N = nodes.size();
    for (const auto& node : nodes) pr_old[node] = init;

    for (int iter = 0; iter < max_iter; ++iter) {
        double max_diff = 0;
        for (const auto& node : nodes) {
            double sum = 0.0;
            // 处理入边节点贡献
            for (const auto& v : inEdges[node]) { // v是当前节点的前驱节点
                if (adjList[v].empty()) {
                    // 出度为0时贡献PR(v)/N
                    sum += pr_old[v] / N;
                } else {
                    // 正常贡献PR(v)/L(v)
                    sum += pr_old[v] / adjList[v].size();
                }
            }
            // 更新公式
            pr_new[node] = (1 - d)/N + d * sum;
            max_diff = max(max_diff, abs(pr_new[node] - pr_old[node]));
        }
        if (max_diff < threshold) break;
        pr_old.swap(pr_new); // 使用swap避免拷贝
    }
    return pr_old;
}

// 随机游走
string randomWalk() {
    if (nodes.empty()) return "";
    vector<string> node_list(nodes.begin(), nodes.end());
    uniform_int_distribution<int> node_dist(0, node_list.size()-1);
    string current = node_list[node_dist(rng)];

    vector<string> path = {current};
    set<pair<string, string>> visited_edges;

    while (true) {
        if (!adjList.count(current) || adjList[current].empty()) break;
        
        // 构建权重分布
        vector<string> candidates;
        vector<int> weights;
        int total = 0;
        for (const auto& [v, w] : adjList[current]) {
            candidates.push_back(v);
            weights.push_back(w);
            total += w;
        }

        // 按权重随机选择
        uniform_int_distribution<int> edge_dist(0, total-1);
        int r = edge_dist(rng);
        int sum = 0;
        string next;
        for (size_t i = 0; i < candidates.size(); ++i) {
            sum += weights[i];
            if (r < sum) {
                next = candidates[i];
                break;
            }
        }

        // 检查重复边
        if (visited_edges.count({current, next})) break;
        visited_edges.insert({current, next});

        path.push_back(next);
        current = next;
    }

    // 转换为字符串
    stringstream ss;
    for (size_t i = 0; i < path.size(); ++i) {
        ss << path[i];
        if (i != path.size()-1) ss << " ";
    }
    return ss.str();
}

// 显示帮助信息
void showHelp() {
    cout << "\n========== Graph Processor Menu ==========\n"
         << "1. Show Graph Structure\n"
         << "2. Query Bridge Words\n"
         << "3. Generate New Text with Bridge Words\n"
         << "4. Calculate Shortest Path\n"
         << "5. Show PageRank Scores\n"
         << "6. Perform Random Walk\n"
         << "7. Exit Program\n"
         << "==========================================\n";
}

// 主函数
int main(int argc, char* argv[]) {
    string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        cout << "Enter text file path: ";
        getline(cin, filename);
    }

    // 处理文件并建图
    auto words = processFile(filename);
    if (words.empty()) {
        cerr << "Error: Failed to process file or file is empty.\n";
        return 1;
    }
    buildGraph(words);
    cout << "Graph built successfully with " << nodes.size() << " nodes.\n";

    while (true) {
        showHelp();
        cout << "Enter your choice (1-7): ";
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: { // 显示图结构
                cout << "\nGraph Structure:\n";
                for (const auto& [u, edges] : adjList) {
                    cout << "[" << u << "] -> ";
                    for (const auto& [v, w] : edges) {
                        cout << v << "(" << w << ") ";
                    }
                    cout << "\n";
                }
                break;
            }
            case 2: { // 查询桥接词
                string w1, w2;
                cout << "Enter two words (separated by space): ";
                cin >> w1 >> w2;
                cin.ignore();

                transform(w1.begin(), w1.end(), w1.begin(), ::tolower);
                transform(w2.begin(), w2.end(), w2.begin(), ::tolower);

                if (!nodes.count(w1) || !nodes.count(w2)) {
                    cout << "No \"" << w1 << "\" or \"" << w2 << "\" in the graph!\n";
                    break;
                }

                auto bridges = findBridgeWords(w1, w2);
                if (bridges.empty()) {
                    cout << "No bridge words from " << w1 << " to " << w2 << "!\n";
                } else {
                    cout << "The bridge words from " << w1 << " to " << w2 << " are: ";
                    for (size_t i = 0; i < bridges.size(); ++i) {
                        if (i == bridges.size()-1) {
                            cout  << bridges[i] << ".\n";
                        } else if (i == bridges.size()-2) {
                            cout << bridges[i] << " ";
                        } else {
                            cout << bridges[i] << ", ";
                        }
                    }
                }
                break;
            }
            case 3: { // 生成新文本
                string input;
                cout << "Enter new text: ";
                getline(cin, input);
                string output = insertBridgeWords(input);
                cout << "Result: " << output << "\n";
                break;
            }
            case 4: { // 计算最短路径
                string w1, w2;
                cout << "Enter two words (separated by space): ";
                cin >> w1 >> w2;
                cin.ignore();

                auto [path, len] = shortestPath(w1, w2);
                if (len == -1) {
                    cout << "\"" << w1 << "\" and \"" << w2 << "\" are not connected!\n";
                } else {
                    cout << "Shortest path (" << len << "): ";
                    for (size_t i = 0; i < path.size(); ++i) {
                        cout << path[i];
                        if (i != path.size()-1) cout << " -> ";
                    }
                    cout << "\n";
                }
                break;
            }
            case 5: { // 显示PageRank
                auto pr = calculatePageRank();
                cout << "\nPageRank Scores:\n";
                vector<pair<string, double>> sorted_pr(pr.begin(), pr.end());
                sort(sorted_pr.begin(), sorted_pr.end(),
                    [](const auto& a, const auto& b) { return a.second > b.second; });

                for (const auto& [node, score] : sorted_pr) {
                    cout << fixed << setprecision(4)
                         << node << ": " << score << "\n";
                }
                break;
            }
            case 6: { // 随机游走
                string walk = randomWalk();
                cout << "Random walk path:\n" << walk << "\n";
                
                ofstream outfile("random_walk.txt");
                if (outfile) {
                    outfile << walk;
                    cout << "Result saved to random_walk.txt\n";
                } else {
                    cerr << "Error saving file!\n";
                }
                break;
            }
            case 7: // 退出
                cout << "Exiting program...\n";
                return 0;
            default:
                cout << "Invalid choice!\n";
        }
    }
}