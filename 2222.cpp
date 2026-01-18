#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <iomanip>
#include <cstdint>

#if __has_include(<bit>)
#  include <bit>
#  define HAS_STD_POPCOUNT 1
#else
#  define HAS_STD_POPCOUNT 0
#endif

#include "GXPass.hpp"

/* ===================== 参数区 ===================== */

static const int THREAD_COUNT = std::thread::hardware_concurrency();
static const int TEST_DURATION_SEC = 30;
static const int REPORT_INTERVAL_SEC = 2;

static const int OUTPUT_LEN = 64;
static const int PREFIX_LEN = 16;

std::string GLOBAL_CHARSET;

/* ===================== 全局状态 ===================== */

std::atomic<uint64_t> total_hashes{0};
std::atomic<bool> stop_flag{false};

std::mutex stat_mutex;
std::vector<uint64_t> bit_count(OUTPUT_LEN * 8, 0);

/* ===================== 兼容 popcount ===================== */

inline int popcount_u8(uint8_t v) {
#if HAS_STD_POPCOUNT
    return std::popcount(static_cast<uint32_t>(v));
#else
    int c = 0;
    while (v) {
        v &= v - 1;
        ++c;
    }
    return c;
#endif
}

/* ===================== 工具函数 ===================== */

std::string random_input(size_t len = 32) {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    static const char table[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::uniform_int_distribution<size_t> dist(0, sizeof(table) - 2);
    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i)
        s.push_back(table[dist(rng)]);
    return s;
}

/* ===================== 工作线程 ===================== */

void worker_thread() {
    while (!stop_flag.load(std::memory_order_relaxed)) {
        std::string input = random_input();
        std::string hash =
            GXPass::fullsafe(input, -1, OUTPUT_LEN, PREFIX_LEN, GLOBAL_CHARSET);

        {
            std::lock_guard<std::mutex> lock(stat_mutex);
            for (size_t i = 0; i < hash.size(); ++i) {
                uint8_t c = static_cast<uint8_t>(hash[i]);
                for (int b = 0; b < 8; ++b) {
                    if (c & (1u << b))
                        bit_count[i * 8 + b]++;
                }
            }
        }

        total_hashes.fetch_add(1, std::memory_order_relaxed);
    }
}

/* ===================== 雪崩效应 ===================== */

void avalanche_test() {
    std::cout << "\n[Avalanche Effect Test]\n";

    const std::string base = "Avalanche_Base_Input";
    const int tests = 128;

    std::string base_hash =
        GXPass::fullsafe(base, -1, OUTPUT_LEN, PREFIX_LEN, GLOBAL_CHARSET);

    const int total_bits = static_cast<int>(base_hash.size() * 8);
    double sum_ratio = 0.0;

    for (int i = 0; i < tests; ++i) {
        std::string mod = base;
        mod[i % mod.size()] ^= static_cast<char>(1u << (i % 8));

        std::string h =
            GXPass::fullsafe(mod, -1, OUTPUT_LEN, PREFIX_LEN, GLOBAL_CHARSET);

        int diff = 0;
        for (size_t j = 0; j < h.size(); ++j) {
            uint8_t x =
                static_cast<uint8_t>(h[j]) ^
                static_cast<uint8_t>(base_hash[j]);
            diff += popcount_u8(x);
        }

        sum_ratio += double(diff) / total_bits;
    }

    std::cout << "Average bit change: "
              << std::fixed << std::setprecision(2)
              << (sum_ratio / tests) * 100.0 << "%\n";
}

/* ===================== 均匀性分析 ===================== */

void bit_bias_report(uint64_t rounds) {
    std::cout << "\n[Bit Uniformity Test]\n";

    double max_bias = 0.0;
    for (auto v : bit_count) {
        double p = double(v) / rounds;
        max_bias = std::max(max_bias, std::abs(p - 0.5));
    }

    std::cout << "Max bit bias deviation: "
              << std::fixed << std::setprecision(3)
              << max_bias * 100.0 << "%\n";
}

/* ===================== 相似输入验证 ===================== */

void similar_input_test() {
    std::cout << "\n[Similar Input Prefix Check]\n";

    const std::vector<std::string> inputs = {
        "password",
        "password1",
        "password2",
        "password3"
    };

    for (const auto& s : inputs) {
        std::string h =
            GXPass::fullsafe(s, -1, OUTPUT_LEN, PREFIX_LEN, GLOBAL_CHARSET);

        std::cout << std::left << std::setw(12)
                  << s << " -> "
                  << h.substr(0, 24) << "...\n";
    }
}

/* ===================== 主程序 ===================== */

int main() {
    std::cout << "GXPass Integrated Security Test\n";
    std::cout << "================================\n";
    std::cout << "Threads        : " << THREAD_COUNT << "\n";
    std::cout << "Duration       : " << TEST_DURATION_SEC << " s\n";
    std::cout << "Report Interval: " << REPORT_INTERVAL_SEC << " s\n";
    std::cout << "Output Length  : " << OUTPUT_LEN << "\n";
    std::cout << "Prefix Length  : " << PREFIX_LEN << "\n\n";

    for (int i = -128; i < 128; ++i)
        GLOBAL_CHARSET.push_back(static_cast<char>(i));

    auto start = std::chrono::steady_clock::now();
    auto last_report = start;

    std::vector<std::thread> workers;
    for (int i = 0; i < THREAD_COUNT; ++i)
        workers.emplace_back(worker_thread);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        if (elapsed >= TEST_DURATION_SEC)
            break;

        if (now - last_report >= std::chrono::seconds(REPORT_INTERVAL_SEC)) {
            uint64_t cnt = total_hashes.load();
            double speed = cnt / (elapsed + 1e-6);

            std::cout << "[Progress] "
                      << elapsed << "s | "
                      << cnt << " hashes | "
                      << static_cast<uint64_t>(speed) << " H/s\n";

            last_report = now;
        }
    }

    stop_flag.store(true);
    for (auto& t : workers)
        t.join();

    uint64_t total = total_hashes.load();

    std::cout << "\n===== Runtime Tests Complete =====\n";
    std::cout << "Total Hashes: " << total << "\n";

    avalanche_test();
    bit_bias_report(total);
    similar_input_test();

    std::cout << "\nAll tests finished.\n";
    return 0;
}
