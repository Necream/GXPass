#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <cmath>
#include "GXPass.hpp"

#define Generate(Input) GXPass::fullsafe(Input,-2)
using namespace std;
using namespace GXPass;

// 全局统计
unordered_map<string, string> outputMap;      // 输出 -> 输入
unordered_map<size_t, long long> freqMap;     // 输出哈希桶 -> 频次
unordered_map<char, long long> charFreqMap;   // 字符 -> 出现次数
mutex mapMutex;

atomic<bool> collisionFound(false);
atomic<long long> totalTests(0);
atomic<long long> collisionCount(0);

int inputLength;      // 输入字符串长度
int threadCount;      // 线程数量
long long maxTests;   // 最大测试数
int bucketCount = 92; // 均匀性分析桶数（质数比较好）

// 生成指定长度的随机字符串
string generateRandomString() {
    static const string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-=[]{}|;':\",./<>?";

    static thread_local random_device rd;
    static thread_local mt19937 generator(rd());
    uniform_int_distribution<> dist(0, (int)chars.size() - 1);

    string result;
    result.reserve(inputLength);
    for (int i = 0; i < inputLength; ++i) {
        result += chars[dist(generator)];
    }
    return result;
}

// 单个线程测试函数
void testThread() {
    while (totalTests < maxTests) {
        string input = generateRandomString();
        string output = Generate(input);

        lock_guard<mutex> lock(mapMutex);

        // 碰撞检测
        auto it = outputMap.find(output);
        if (it != outputMap.end() && it->second != input) {
            collisionFound = true;
            collisionCount++;
            cerr << "\n发现碰撞!\n输入1: " << it->second
                 << "\n输入2: " << input
                 << "\n输出:   " << output << endl;
        }
        outputMap[output] = input;

        // 均匀性统计
        size_t h = hash<string>{}(output) % bucketCount;
        freqMap[h]++;

        // 字符统计
        for (char c : output) charFreqMap[c]++;

        totalTests++;
    }
}

// 卡方统计
double computeChiSquare() {
    lock_guard<mutex> lock(mapMutex);
    if (totalTests == 0) return 0.0;

    double expected = (double)totalTests / bucketCount;
    double chi2 = 0.0;
    for (auto &kv : freqMap) {
        double diff = kv.second - expected;
        chi2 += diff * diff / expected;
    }
    long long missing = bucketCount - (long long)freqMap.size();
    chi2 += missing * (expected * expected) / expected;
    return chi2;
}

// 输出热门和冷门字符
void printHotColdChars() {
    lock_guard<mutex> lock(mapMutex);
    if (charFreqMap.empty()) return;

    vector<pair<char,long long>> charVec(charFreqMap.begin(), charFreqMap.end());
    sort(charVec.begin(), charVec.end(), [](auto &a, auto &b){ return a.second > b.second; });
    cout << "\n热门字符: ";
    for (int i = 0; i < min(5,(int)charVec.size()); ++i)
        cout << charVec[i].first << "(" << charVec[i].second << ") ";
    sort(charVec.begin(), charVec.end(), [](auto &a, auto &b){ return a.second < b.second; });
    cout << "\n冷门字符: ";
    for (int i = 0; i < min(5,(int)charVec.size()); ++i)
        cout << charVec[i].first << "(" << charVec[i].second << ") ";
    cout << endl;
}

int main(int argc, char* argv[]) {
    inputLength = 11;
    threadCount = thread::hardware_concurrency();
    maxTests = 2000000;

    if (argc > 1) inputLength = stoi(argv[1]);
    if (argc > 2) threadCount = stoi(argv[2]);
    if (argc > 3) maxTests = stoll(argv[3]);

    cout << "GXPass 均匀性 & 碰撞分析" << endl;
    cout << "输入长度: " << inputLength
         << " | 线程数量: " << threadCount
         << " | 最大测试数: " << maxTests
         << " | 桶数量: " << bucketCount
         << " | 输出长度：" << Generate(generateRandomString()).size() << endl;
    cout << "开始测试..." << endl;

    auto start = chrono::high_resolution_clock::now();

    // 启动线程
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i)
        threads.emplace_back(testThread);

    // 主线程监控进度和输出热门/冷门字符
    while (totalTests < maxTests) {
        this_thread::sleep_for(chrono::seconds(2));

        long long tested = totalTests;
        double chi2 = computeChiSquare();
        double elapsed = chrono::duration<double>(
            chrono::high_resolution_clock::now() - start
        ).count();

        cout << "\r已完成: " << tested
             << " | 不同输出: " << outputMap.size()
             << " | 碰撞数: " << collisionCount
             << " | 卡方: " << chi2
             << " | 速度: " << (tested / elapsed) << " 次/秒"
             << flush;

        printHotColdChars();
    }

    collisionFound = true;
    for (auto &t : threads) t.join();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "\n\n测试结束" << endl;
    cout << "总测试数: " << totalTests << endl;
    cout << "不同输出: " << outputMap.size() << endl;
    cout << "碰撞数: " << collisionCount << endl;
    cout << "卡方值: " << computeChiSquare() << endl;
    cout << "耗时: " << duration / 1000.0 << "s" << endl;
    cout << "速度: " << (totalTests * 1000.0 / duration) << " 次/秒" << endl;

    // 最终热门/冷门字符
    printHotColdChars();

    return 0;
}
