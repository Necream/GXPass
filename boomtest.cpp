#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>
#include "GXPass.hpp"

using namespace std;
using namespace GXPass;

// 全局变量用于记录输出和碰撞信息
unordered_map<string, string> outputMap;  // 输出 -> 输入
mutex mapMutex;
atomic<bool> collisionFound(false);
atomic<long long> totalTests(0);
int inputLength;  // 输入字符串长度
int threadCount;  // 线程数量

// 生成指定长度的随机字符串
string generateRandomString() {
    static const string chars = "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789"
                                "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    
    static thread_local random_device rd;
    static thread_local mt19937 generator(rd());
    uniform_int_distribution<> dist(0, (int)chars.size() - 1);
    
    string result;
    for (int i = 0; i < inputLength; ++i) {
        result += chars[dist(generator)];
    }
    return result;
}

// 单个线程的测试函数
void testThread() {
    while (!collisionFound) {
        // 生成随机输入
        string input = generateRandomString();
        // 计算输出
        string output = fullsafe(input);
        
        // 检查是否存在碰撞
        mapMutex.lock();
        if (outputMap.find(output) != outputMap.end()) {
            // 找到碰撞
            if (outputMap[output] != input) {  // 排除相同输入的情况
                cout << "\n发现碰撞!" << endl;
                cout << "输入1: " << outputMap[output] << endl;
                cout << "输入2: " << input << endl;
                cout << "输出:   " << output << endl;
                collisionFound = true;
                mapMutex.unlock();
                return;
            }
        } else {
            // 记录新的输出-输入对
            outputMap[output] = input;
        }
        totalTests++;
        mapMutex.unlock();
        
        // 定期输出进度 (每10000次测试)
        if (totalTests % 10000 == 0) {
            mapMutex.lock();
            cout << "\r已测试: " << totalTests << " 个输入 | 已记录输出数: " << outputMap.size();
            cout.flush();
            mapMutex.unlock();
        }
    }
}

int main(int argc, char* argv[]) {
    // 默认参数
    inputLength = 11;    // 输入长度，可修改
    threadCount = thread::hardware_concurrency();  // 使用CPU核心数作为线程数
    long long maxTests = 10000000;  // 最大测试数量，可修改
    
    // 从命令行参数获取配置 (可选)
    if (argc > 1) inputLength = stoi(argv[1]);
    if (argc > 2) threadCount = stoi(argv[2]);
    if (argc > 3) maxTests = stoll(argv[3]);
    
    cout << "GXPass 碰撞测试程序" << endl;
    cout << "输入长度: " << inputLength << endl;
    cout << "线程数量: " << threadCount << endl;
    cout << "最大测试数: " << maxTests << endl;
    cout << "开始测试..." << endl;
    
    auto start = chrono::high_resolution_clock::now();
    
    // 创建并启动线程
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(testThread);
    }
    
    // 监控测试进度，达到最大测试数时停止
    while (totalTests < maxTests && !collisionFound) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    // 结束所有线程
    collisionFound = true;
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
    cout << "\n\n测试结束" << endl;
    cout << "总测试数: " << totalTests << endl;
    cout << "耗时: " << duration << "ms (" << duration / 1000.0 << "s)" << endl;
    cout << "测试速度: " << (totalTests * 1000.0 / duration) << " 次/秒" << endl;
    if (!collisionFound) {
        cout << "未发现碰撞" << endl;
    }
    
    return 0;
}
