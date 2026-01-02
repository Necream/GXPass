#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>
#include <bitset>
// #include <bit>
#include <algorithm>
#include "GXPass.hpp"

using Clock = std::chrono::steady_clock;

// ================= CONFIG =================

constexpr size_t SAMPLE_COUNT = 10000;

// Thread count must be runtime initialized
size_t THREAD_COUNT = 8;

// =========================================

std::mutex printMutex;

// ===== DECLARE YOUR FUNCTION HERE =====
// std::string fullsafe(const std::string& data);
// =====================================

// ---------- Utility ----------

std::string random_string(size_t len) {
    static const char pool[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-={}[]|:;\"'<>,.?/";

    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, sizeof(pool) - 2);

    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i)
        s.push_back(pool[dist(rng)]);
    return s;
}

uint64_t hamming_distance(const std::string& a, const std::string& b) {
    uint64_t diff = 0;
    size_t len = std::min(a.size(), b.size());
    for (size_t i = 0; i < len; ++i) {
        unsigned char x =
            static_cast<unsigned char>(a[i] ^ b[i]);
        diff += std::bitset<8>(x).count(); // Portable alternative
    }
    return diff;
}

// ---------- Distribution Test ----------

void test_distribution() {
    std::unordered_map<char, std::atomic<uint64_t>> freq;
    std::atomic<uint64_t> progress{0};

    auto start = Clock::now();

    auto worker = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            auto out = GXPass::fullsafe(random_string(16),-1,128,128);
            for (char c : out)
                freq[c]++;
            progress++;
        }
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_COUNT; ++i)
        threads.emplace_back(worker, SAMPLE_COUNT / THREAD_COUNT);

    while (progress < SAMPLE_COUNT) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            Clock::now() - start).count();

        double pct = (double)progress / SAMPLE_COUNT * 100.0;
        double ops = elapsed ? progress / elapsed : 0;

        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "[Distribution] "
                  << std::fixed << std::setprecision(2)
                  << pct << "% | "
                  << progress << "/" << SAMPLE_COUNT
                  << " | " << ops << " ops/s\n";
    }

    for (auto& t : threads) t.join();

    uint64_t total = 0;
    for (auto& p : freq)
        total += p.second.load();

    std::cout << "\nCharacter Frequency:\n";
    for (auto& p : freq) {
        double percent =
            (double)p.second / total * 100.0;
        std::cout << p.first << " : "
                  << percent << "%\n";
    }
}

// ---------- Avalanche Test ----------

void test_avalanche() {
    std::atomic<uint64_t> totalBits{0};
    std::atomic<uint64_t> progress{0};

    auto start = Clock::now();

    auto worker = [&](size_t count) {
        for (size_t i = 0; i < count; ++i) {
            std::string base = random_string(16);
            std::string modified = base;
            modified[0] ^= 1;

            auto a = GXPass::fullsafe(base,-1,128,128);
            auto b = GXPass::fullsafe(modified,-1,128,128);

            totalBits += hamming_distance(a, b);
            progress++;
        }
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_COUNT; ++i)
        threads.emplace_back(worker, SAMPLE_COUNT / THREAD_COUNT);

    while (progress < SAMPLE_COUNT) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            Clock::now() - start).count();

        double pct = (double)progress / SAMPLE_COUNT * 100.0;
        double ops = elapsed ? progress / elapsed : 0;

        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "[Avalanche] "
                  << std::fixed << std::setprecision(2)
                  << pct << "% | "
                  << progress << "/" << SAMPLE_COUNT
                  << " | " << ops << " ops/s\n";
    }

    for (auto& t : threads) t.join();

    double avg = (double)totalBits / SAMPLE_COUNT;
    std::cout << "\nAverage flipped bits: "
              << avg << "\n";
}

// ---------- Main ----------

int main() {
    THREAD_COUNT = std::thread::hardware_concurrency();
    if (THREAD_COUNT == 0)
        THREAD_COUNT = 8;

    std::cout << "GXPass Full Strength Test\n";
    std::cout << "Threads: " << THREAD_COUNT << "\n";
    std::cout << "Samples per test: " << SAMPLE_COUNT << "\n\n";

    std::cout << "=== Distribution Test ===\n";
    test_distribution();

    std::cout << "\n=== Avalanche Test ===\n";
    test_avalanche();

    std::cout << "\nAll tests completed.\n";
    return 0;
}
