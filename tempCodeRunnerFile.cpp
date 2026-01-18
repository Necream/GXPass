#include <iostream>
#include <string>
#include <bitset>
#include <algorithm>

#include "GXPass.hpp"

// 统计 bit 差异
static size_t bit_diff(const std::string& a, const std::string& b) {
    size_t diff = 0;
    size_t n = std::min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        diff += std::bitset<8>(
            static_cast<unsigned char>(a[i] ^ b[i])
        ).count();
    }
    return diff;
}

int main() {
    using namespace GXPass;

    // === 固定测试参数（与你 encrypt 中一致）===
    const std::string base_input =
        "| PassHash: test_password | DataHash: test_data_hash";

    const std::string numbercharset = "123456789";
    const int version = -1;
    const int passLen = 600;
    const int preLen  = 16;

    // 原始输出
    std::string ref = fullsafe(
        base_input, version, passLen, preLen, numbercharset
    );

    const size_t total_bits = ref.size() * 8;
    double sum_rate = 0.0;
    double min_rate = 1.0;
    size_t tests = 0;

    // === 逐字节逐 bit 翻转 ===
    for (size_t i = 0; i < base_input.size(); ++i) {
        for (int bit = 0; bit < 8; ++bit) {
            std::string modified = base_input;
            modified[i] ^= (1 << bit);

            std::string out = fullsafe(
                modified, version, passLen, preLen, numbercharset
            );

            size_t diff = bit_diff(ref, out);
            double rate = static_cast<double>(diff) / total_bits;

            sum_rate += rate;
            min_rate = std::min(min_rate, rate);
            ++tests;
        }
    }

    std::cout << "=====================================\n";
    std::cout << "GXPass fullsafe Avalanche Test\n";
    std::cout << "Output length  : " << ref.size() << " bytes\n";
    std::cout << "Total tests    : " << tests << "\n";
    std::cout << "Average rate   : "
              << (sum_rate / tests) * 100.0 << "%\n";
    std::cout << "Minimum rate   : "
              << min_rate * 100.0 << "%\n";
    std::cout << "=====================================\n";

    return 0;
}
