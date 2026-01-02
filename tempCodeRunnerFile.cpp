#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <atomic>

#include "GXPass.hpp"

using namespace std;

/* Utility */

int hamming_distance(const string& a, const string& b) {
    int d = 0;
    size_t n = min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        if (a[i] != b[i]) d++;
    }
    d += abs((int)a.size() - (int)b.size());
    return d;
}

/* Thread-safe merge */

mutex freq_mutex;
mutex set_mutex;

/* Test 1: Character distribution (multithreaded) */

void dist_worker(int start, int end, int passlen, map<char, long long>& local) {
    for (int i = start; i < end; ++i) {
        string input = "input_" + to_string(i);
        string out = GXPass::fullsafe(input, -1, passlen);
        for (char c : out) local[c]++;
    }
}

void test_distribution() {
    cout << "=== Character Distribution Test ===" << endl;

    const int samples = 50000;
    const int passlen = 128;
    int threads = thread::hardware_concurrency();
    if (threads <= 0) threads = 4;

    vector<thread> pool;
    vector<map<char, long long>> locals(threads);

    int block = samples / threads;

    for (int t = 0; t < threads; ++t) {
        int start = t * block;
        int end = (t == threads - 1) ? samples : start + block;
        pool.emplace_back(dist_worker, start, end, passlen, ref(locals[t]));
    }

    for (auto& th : pool) th.join();

    map<char, long long> freq;
    for (auto& m : locals) {
        for (auto& p : m) freq[p.first] += p.second;
    }

    double total = (double)samples * passlen;
    for (char c : GXPass::charset) {
        double p = freq[c] / total;
        cout << c << " : " << fixed << setprecision(6) << p << endl;
    }

    cout << endl;
}

/* Test 2: Avalanche (single-thread, deterministic) */

void test_avalanche() {
    cout << "=== Avalanche Test ===" << endl;

    string base = "password_example_123456";
    string base_out = GXPass::fullsafe(base);

    for (size_t i = 0; i < base.size(); ++i) {
        string modified = base;
        modified[i] ^= 1;

        string mod_out = GXPass::fullsafe(modified);
        int diff = hamming_distance(base_out, mod_out);
        double ratio = (double)diff / base_out.size();

        cout << "flip@" << i << " ratio=" << fixed << setprecision(3) << ratio << endl;
    }

    cout << endl;
}

/* Test 3: Collision test (multithreaded) */

void collision_worker(int start, int end, int passlen,
                      unordered_set<string>& global,
                      atomic<int>& collisions) {
    unordered_set<string> local;

    for (int i = start; i < end; ++i) {
        string input = "item_" + to_string(i);
        string out = GXPass::fullsafe(input, -1, passlen);
        local.insert(out);
    }

    lock_guard<mutex> lock(set_mutex);
    for (auto& s : local) {
        if (global.count(s)) collisions++;
        else global.insert(s);
    }
}

void test_collision() {
    cout << "=== Collision Test ===" << endl;

    const int samples = 200000;
    const int passlen = 64;
    int threads = thread::hardware_concurrency();
    if (threads <= 0) threads = 4;

    unordered_set<string> global;
    atomic<int> collisions(0);
    vector<thread> pool;

    int block = samples / threads;

    for (int t = 0; t < threads; ++t) {
        int start = t * block;
        int end = (t == threads - 1) ? samples : start + block;
        pool.emplace_back(collision_worker, start, end, passlen,
                          ref(global), ref(collisions));
    }

    for (auto& th : pool) th.join();

    cout << "samples=" << samples << endl;
    cout << "collisions=" << collisions.load() << endl;
    cout << endl;
}

/* Test 4: Performance (single-thread, precise) */

void test_performance() {
    cout << "=== Performance Test ===" << endl;

    vector<int> lengths = {8, 16, 32, 64};

    for (int len : lengths) {
        string s(len, 'A');

        auto t1 = chrono::high_resolution_clock::now();
        GXPass::fullsafe(s);
        auto t2 = chrono::high_resolution_clock::now();

        double ms = chrono::duration<double, milli>(t2 - t1).count();
        cout << "input_len=" << len
             << " time_ms=" << fixed << setprecision(3) << ms << endl;
    }

    cout << endl;
}

/* Main */

int main() {
    cout << "GXPass Strength Test Tool (Multithreaded)" << endl << endl;

    test_distribution();
    test_avalanche();
    test_collision();
    test_performance();

    cout << "Done." << endl;
    return 0;
}
