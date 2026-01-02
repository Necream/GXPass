#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include "GXPass.hpp"

using namespace std;
using namespace GXPass;

// 生成指定长度的随机字符串（包含大小写字母、数字和常见符号）
string generateRandomString(int length) {
    const string chars = "abcdefghijklmnopqrstuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "0123456789"
                         "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, (int)chars.size() - 1);
    
    string result;
    for (int i = 0; i < length; ++i) {
        result += chars[dist(generator)];
    }
    return result;
}

// 修改字符串的第n位字符为随机字符
string modifyOneCharacter(const string& s, int position) {
    if (position < 0 || position >= (int)s.size()) return s;
    
    const string chars = "abcdefghijklmnopqrstuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "0123456789"
                         "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, (int)chars.size() - 1);
    
    string result = s;
    // 确保修改后的字符与原字符不同
    char original = result[position];
    do {
        result[position] = chars[dist(generator)];
    } while (result[position] == original);
    
    return result;
}

// 计算两个字符串的差异率（不同字符数/总长度）
double calculateDifferenceRate(const string& a, const string& b) {
    if (a.empty() || b.empty()) return 0.0;
    
    // 取较短的长度进行比较
    int minLength = min((int)a.size(), (int)b.size());
    int diffCount = 0;
    
    for (int i = 0; i < minLength; ++i) {
        if (a[i] != b[i]) {
            diffCount++;
        }
    }
    
    // 长度不同的部分也视为差异
    diffCount += abs((int)a.size() - (int)b.size());
    
    // 以较长的长度作为基准计算差异率
    int maxLength = max((int)a.size(), (int)b.size());
    return static_cast<double>(diffCount) / maxLength;
}

int main() {
    double minAvalancheRate = 1;
    double maxAvalancheRate = 0;
    double avrageAvalancheRate = -1;
    // 测试不同长度的输入（从10位到50位，步长为10）
    for (int length = 10,j=0; length <= 500; length += 10,j++) {
        cout << "测试输入长度: " << length << endl;
        
        // 生成原始输入
        string original = generateRandomString(length);
        cout << "原始输入前" << min(10, length) << "位: " << original.substr(0, 10);
        if (length > 10) cout << "...";
        cout << endl;
        
        // 生成修改1位的输入（随机选择修改位置）
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> dist(0, length - 1);
        int modifyPos = dist(generator);
        string modified = modifyOneCharacter(original, modifyPos);
        cout << "修改位置: " << modifyPos << " (原始字符: " << original[modifyPos] 
             << ", 修改后: " << modified[modifyPos] << ")" << endl;
        
        // 计算原始输出和修改后的输出
        auto start = chrono::high_resolution_clock::now();
        string originalOutput = fullsafe(original);
        string modifiedOutput = fullsafe(modified);
        auto end = chrono::high_resolution_clock::now();
        
        // 计算雪崩率（差异率）
        double avalancheRate = calculateDifferenceRate(originalOutput, modifiedOutput);
        
        // 输出结果
        cout << "原始输出长度: " << originalOutput.size() 
             << ", 修改后输出长度: " << modifiedOutput.size() << endl;
        cout << "雪崩率: " << avalancheRate * 100 << "%" << endl;
        if(avrageAvalancheRate<0){
            avrageAvalancheRate=avalancheRate;
        }else{
            avrageAvalancheRate=(avrageAvalancheRate+avalancheRate)/2;
        }
        if(avalancheRate<minAvalancheRate){
            minAvalancheRate=avalancheRate;
        }
        if(avalancheRate>maxAvalancheRate){
            maxAvalancheRate=avalancheRate;
        }
        cout << "计算耗时: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
        cout << "----------------------------------------" << endl;
    }
    cout << "最低雪崩率: " << minAvalancheRate * 100 << "%" << endl;
    cout << "最高雪崩率: " << maxAvalancheRate * 100 << "%" << endl;
    cout << "平均雪崩率: " << avrageAvalancheRate * 100 << "%" << endl;
    return 0;
}
