#include <iostream>
#include <string>
#include <random>
#include <iomanip>
#include "GXPass.hpp"

// 计算两个字符串输出变化占比
double avalancheRatio(const std::string& hash1, const std::string& hash2) {
    if (hash1.size() != hash2.size()) return -1.0; // 输出长度不同返回错误
    size_t changes = 0;
    for (size_t i = 0; i < hash1.size(); i++) {
        if (hash1[i] != hash2[i]) changes++;
    }
    return static_cast<double>(changes) / hash1.size();
}

// 随机改变输入字符串的一个字符（MSVC兼容）
std::string randomMutate(const std::string& input, std::mt19937& rng) {
    if (input.empty()) return input;
    std::uniform_int_distribution<size_t> dist_pos(0, input.size() - 1);
    std::uniform_int_distribution<int> dist_char(32, 126); // 可打印 ASCII
    std::string mutated = input;
    mutated[dist_pos(rng)] = static_cast<char>(dist_char(rng));
    return mutated;
}

int main() {
    std::mt19937 rng(std::random_device{}());

    // 输入长度测试范围
    const int minLen = 1;
    const int maxLen = 1000;
    const int step = 1;
    const int trials = 1000; // 每个长度随机修改次数

    std::cout << "InputLen,AverageAvalanche%\n";

    for (int len = minLen; len <= maxLen; len += step) {
        // 构造初始随机字符串
        std::string input;
        std::uniform_int_distribution<int> dist_char(32, 126);
        for (int i = 0; i < len; i++) {
            input += static_cast<char>(dist_char(rng));
        }

        std::string hash_original = GXPass::compile(input);

        double sum_ratio = 0.0;
        for (int t = 0; t < trials; t++) {
            std::string mutated = randomMutate(input, rng);
            std::string hash_mutated = GXPass::compile(mutated);
            double ratio = avalancheRatio(hash_original, hash_mutated);
            if (ratio >= 0) sum_ratio += ratio;
        }

        double average_ratio = sum_ratio / trials;
        std::cout << len << "," << std::fixed << std::setprecision(4)
                  << (average_ratio * 100) << "\n";
    }

    return 0;
}
