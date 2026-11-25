#include "Input.h"
#include "WorkRecord.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <cctype>

using namespace std;

static string trim(const string& value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

int inputInt(const string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.good() && cin.peek() == '\n' && value >= minValue && value <= maxValue) {
            cin.ignore(10000, '\n');
            return value;
        }
        cin.clear();
        cout << "\nнеправильный ввод данных. повторите попытку\n";
        cin.ignore(10000, '\n');
    }
}

double inputDouble(const string& prompt, double minValue, double maxValue) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.good() && cin.peek() == '\n' && value >= minValue && value <= maxValue) {
            cin.ignore(10000, '\n');
            return value;
        }
        cin.clear();
        cout << "\nнеправильный ввод данных. введите число в допустимом диапазоне\n";
        cin.ignore(10000, '\n');
    }
}

string inputString(const string& prompt, int maxLength) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        value = trim(value);
        if (cin.good() && !value.empty() && static_cast<int>(value.length()) <= maxLength)
            return value;
        cin.clear();
        cout << "\nстрока не должна быть пустой и должна содержать не более "
            << maxLength << " символов\n";
    }
}

bool inputYesNo(const string& prompt) {
    return inputInt(prompt + " (1 - да, 0 - нет)-> ", 0, 1) == 1;
}
