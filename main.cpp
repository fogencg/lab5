#define _HAS_STD_BYTE 0
#include "MedicalRegistry.h"
#include <iostream>
#include <Windows.h>

using namespace std;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    MedicalRegistry registry;
    registry.run();
    return 0;
}
