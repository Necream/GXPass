#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include "GXPass.hpp"

// 生成一个完全随机字符串（长度可调）
std::string generateRandomInput(int length = 6) {
    static std::mt19937 rng(std::random_device{}());
    static const std::string charset = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789";
    std::uniform_int_distribution<int> dist(0, charset.size() - 1);

    std::string result;
    for (int i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }
    return result;
}

int main() {
    std::string userInput;
    std::cout << "Enter your input string: ";
    std::cin >> userInput;

    int version = -1;
    std::string targetPass = GXPass::fullsafe(userInput, version);
    std::cout << "[*] Target FinalPass: " << targetPass << "\n";

    int count = 0;
    while (true) {
        std::string randomInput = generateRandomInput();
        std::string result = GXPass::fullsafe(randomInput, version);

        if (result == targetPass) {
            std::cout << "[!] Collision found at count " << count << ":\n";
            std::cout << "    Random input : " << randomInput << "\n";
            std::cout << "    FinalPass    : " << result << "\n";
            break;
        }

        if (count % 1000 == 0) {
            std::cout << "[+] Checked " << count << " random inputs. No match yet.\n";
        }

        count++;
    }

    return 0;
}