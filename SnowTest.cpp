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

// 随机改变输入字符串的一个字符（MSVC 兼容）
std::string randomMutate(const std::string& input, std::mt19937& rng) {
    if (input.empty()) return input;
    std::uniform_int_distribution<size_t> dist_pos(0, input.size() - 1);
    std::uniform_int_distribution<int> dist_char(32, 126); // 可打印 ASCII
    std::string mutated = input;
    mutated[dist_pos(rng)] = static_cast<char>(dist_char(rng));
    return mutated;
}

int main() {
    std::string input;
    std::cout << "Enter the input string: ";
    std::getline(std::cin, input);

    std::string hash_original = GXPass::fullsafe(input, -1, 256, 256);

    const int trials = 1; // 多次随机修改
    double sum_ratio = 0.0;

    std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < trials; i++) {
        std::string mutated = randomMutate(input, rng);
        std::string hash_mutated = GXPass::fullsafe(mutated, -1, 256, 256);
        double ratio = avalancheRatio(hash_original, hash_mutated);
        if (ratio >= 0) sum_ratio += ratio;
    }

    double average_ratio = sum_ratio / trials;
    std::cout << "Average avalanche ratio over " << trials << " trials: "
              << std::fixed << std::setprecision(4)
              << (average_ratio * 100) << "%\n";

    return 0;
}
