#pragma once

#include <string>

using namespace std;

int inputInt(const string& prompt, int minValue, int maxValue);
double inputDouble(const string& prompt, double minValue, double maxValue);
string inputString(const string& prompt, int maxLength);
bool inputYesNo(const string& prompt);
