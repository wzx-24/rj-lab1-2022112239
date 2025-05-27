#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// 全局变量声明（如果需要外部访问）
extern std::unordered_map<std::string, std::unordered_map<std::string, int>> adjList;
extern std::unordered_map<std::string, std::vector<std::string>> inEdges;
extern std::unordered_set<std::string> nodes;

// 函数声明
std::vector<std::string> processFile(const std::string& filename);
void buildGraph(const std::vector<std::string>& words);
void exportGraphToDot(const std::string& filename);
std::vector<std::string> findBridgeWords(const std::string& w1, const std::string& w2);
std::string insertBridgeWords(const std::string& input);
std::pair<std::vector<std::string>, int> shortestPath(const std::string& start, const std::string& end);
std::unordered_map<std::string, double> calculatePageRank(double d = 0.85, double threshold = 1e-6, int max_iter = 1000);
std::string randomWalk();
void showHelp();