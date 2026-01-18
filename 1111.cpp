#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <chrono>

#include "GXPass.hpp"

using namespace std;
using namespace GXPass;

int hamming(const string& a, const string& b) {
    int d = 0;
    int n = min(a.size(), b.size());
    for (int i = 0; i < n; i++) {
        unsigned char x = a[i] ^ b[i];
        d += __popcnt(x);
    }
    return d;
}

int main() {
    cout << "GXPass mini avalanche test\n";

    string input = "Necream!"; // 8 bytes
    cout << "Input: " << input << "\n";

    auto start = chrono::high_resolution_clock::now();

    string charsets;
    for(int i=-128;i<128;i++){
        charsets+=static_cast<char>(i);
    }
    string base = fullsafe(input, -1, 64, 32, charsets);
    int totalBits = base.size() * 8;

    int tests = 0;
    double sumRate = 0.0;
    double minRate = 1.0;
    double maxRate = 0.0;

    // 只测前 2 个字符
    for (int i = 0; i < 2; i++) {
        for (int bit = 0; bit < 4; bit++) {
            string modified = input;
            modified[i] ^= (1 << bit);

            string out = fullsafe(modified, -1, 64, 32);

            int diff = hamming(base, out);
            double rate = (double)diff / totalBits;

            sumRate += rate;
            minRate = min(minRate, rate);
            maxRate = max(maxRate, rate);
            tests++;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    double sec = chrono::duration<double>(end - start).count();

    cout << "--------------------------------\n";
    cout << "Tests       : " << tests << "\n";
    cout << "Avg rate    : " << (sumRate / tests) * 100 << "%\n";
    cout << "Min rate    : " << minRate * 100 << "%\n";
    cout << "Max rate    : " << maxRate * 100 << "%\n";
    cout << "Time used   : " << sec << " s\n";

    return 0;
}
