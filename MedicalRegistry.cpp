#include "MedicalRegistry.h"
#include "Input.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <sstream>

using namespace std;

static string escapeField(const string& value) {
    string result;
    for (char ch : value) {
        if (ch == '\\' || ch == '|') result += '\\';
        result += ch;
    }
    return result;
}

static vector<string> splitRecordLine(const string& line) {
    vector<string> fields;
    string field;
    bool escaped = false;
    for (char ch : line) {
        if (escaped) {
            field += ch;
            escaped = false;
        }
        else if (ch == '\\') escaped = true;
        else if (ch == '|') {
            fields.push_back(field);
            field.clear();
        }
        else field += ch;
    }
    if (escaped) field += '\\';
    fields.push_back(field);
    return fields;
}

MedicalRegistry::MedicalRegistry() {
    loadFromFile(1);
    loadFromFile(2);
    loadFromFile(3);
}

string MedicalRegistry::getFileName(int containerType) const {
    if (containerType == 1) return "queue.txt";
    if (containerType == 2) return "stack.txt";
    return "priority_queue.txt";
}

void MedicalRegistry::loadFromFile(int containerType) {
    const string fileName = getFileName(containerType);
    ifstream file(fileName);
    if (!file.is_open()) return;

    vector<WorkRecord> records;
    string line;
    int skipped = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        vector<string> fields = splitRecordLine(line);
        if (fields.size() != 8) { ++skipped; continue; }

        int id, day, month, year;
        double hours;
        istringstream idStream(fields[0]), dayStream(fields[4]), monthStream(fields[5]),
            yearStream(fields[6]), hoursStream(fields[7]);
        if (!(idStream >> id) || !(dayStream >> day) || !(monthStream >> month) ||
            !(yearStream >> year) || !(hoursStream >> hours) || id <= 0 ||
            !WorkRecord::isValidDate(day, month, year) || hours < 0.1 || hours > 24.0) {
            ++skipped;
            continue;
        }
        records.push_back(WorkRecord(id, fields[1], fields[2], fields[3],
            day, month, year, hours));
    }
    setRecords(containerType, records);
    if (!records.empty())
        cout << "загружено записей из файла " << fileName << ": " << records.size() << "\n";
    if (skipped > 0)
        cout << "пропущено поврежденных строк в файле " << fileName << ": " << skipped << "\n";
}

vector<WorkRecord> MedicalRegistry::getRecords(int containerType) const {
    vector<WorkRecord> result;
    if (containerType == 1) {
        queue<WorkRecord> copy = workQueue;
        while (!copy.empty()) { result.push_back(copy.front()); copy.pop(); }
    }
    else if (containerType == 2) {
        stack<WorkRecord> copy = workStack;
        while (!copy.empty()) { result.push_back(copy.top()); copy.pop(); }
    }
    else {
        priority_queue<WorkRecord, vector<WorkRecord>, PriorityCompare> copy = priorityWorkQueue;
        while (!copy.empty()) { result.push_back(copy.top()); copy.pop(); }
    }
    return result;
}

void MedicalRegistry::setRecords(int containerType, const vector<WorkRecord>& records) {
    if (containerType == 1) {
        workQueue = queue<WorkRecord>();
        for (const WorkRecord& record : records) workQueue.push(record);
    }
    else if (containerType == 2) {
        workStack = stack<WorkRecord>();
        for (auto it = records.rbegin(); it != records.rend(); ++it) workStack.push(*it);
    }
    else {
        priorityWorkQueue = priority_queue<WorkRecord, vector<WorkRecord>, PriorityCompare>();
        for (const WorkRecord& record : records) priorityWorkQueue.push(record);
    }
}

bool MedicalRegistry::containsId(int containerType, int id) const {
    vector<WorkRecord> records = getRecords(containerType);
    for (const WorkRecord& record : records)
        if (record.getId() == id) return true;
    return false;
}

void MedicalRegistry::printTable(const vector<WorkRecord>& records) const {
    const int wId = 6, wName = 40, wPosition = 18, wDepartment = 18, wDate = 12, wHours = 9, wOvertime = 10;
    auto line = [&]() {
        cout << string(wId, '-') << "+" << string(wName + 2, '-') << "+"
            << string(wPosition + 2, '-') << "+" << string(wDepartment + 2, '-') << "+"
            << string(wDate + 2, '-') << "+" << string(wHours, '-') << "+"
            << string(wOvertime, '-') << "\n";
        };
    cout << "\n" << left << setw(wId) << "id" << "+"
        << setw(wName + 2) << " фио" << "+"
        << setw(wPosition + 2) << " должность" << "+"
        << setw(wDepartment + 2) << " отделение" << "+"
        << setw(wDate + 2) << " дата" << "+"
        << setw(wHours) << " часы" << "+"
        << setw(wOvertime) << " сверхур." << "\n";
    line();
    if (records.empty()) cout << "записи отсутствуют\n";
    for (const WorkRecord& record : records) {
        cout << left << setw(wId) << record.getId() << "+"
            << setw(wName + 2) << record.getFullName() << "+"
            << setw(wPosition + 2) << record.getPosition() << "+"
            << setw(wDepartment + 2) << record.getDepartment() << "+"
            << setw(wDate + 2) << record.getDateString() << "+"
            << right << setw(wHours) << fixed << setprecision(1) << record.getHours() << "+"
            << setw(wOvertime) << record.getOvertime() << "\n";
    }
    line();
    double totalHours = 0.0, totalOvertime = 0.0;
    for (const WorkRecord& record : records) {
        totalHours += record.getHours();
        totalOvertime += record.getOvertime();
    }
    cout << "количество записей: " << records.size()
        << ", всего часов: " << fixed << setprecision(1) << totalHours
        << ", сверхурочно: " << totalOvertime << "\n";
}

void MedicalRegistry::addRecord(int containerType) {
    cout << "\nдобавление записи\n";
    int id;
    while (true) {
        id = inputInt("введите id записи (1-999999)-> ", 1, 999999);
        if (!containsId(containerType, id)) break;
        cout << "запись с таким id уже существует в выбранном контейнере\n";
    }
    string fullName = inputString("введите фио медицинского работника-> ", 60);
    string position = inputString("введите должность-> ", 40);
    string department = inputString("введите отделение-> ", 40);
    int day, month, year;
    while (true) {
        day = inputInt("введите день-> ", 1, 31);
        month = inputInt("введите месяц-> ", 1, 12);
        year = inputInt("введите год (2000-2100)-> ", 2000, 2100);
        if (WorkRecord::isValidDate(day, month, year)) break;
        cout << "такой календарной даты не существует. повторите ввод\n";
    }
    double hours = inputDouble("введите количество отработанных часов (0.1-24.0)-> ", 0.1, 24.0);
    WorkRecord record(id, fullName, position, department, day, month, year, hours);
    vector<WorkRecord> records = getRecords(containerType);
    records.push_back(record);
    setRecords(containerType, records);
    saveToFile(containerType);
    cout << "запись успешно добавлена\n";
}

void MedicalRegistry::deleteRecord(int containerType) {
    if (getRecords(containerType).empty()) { cout << "контейнер пуст\n"; return; }
    int id = inputInt("введите id записи для удаления-> ", 1, 999999);
    vector<WorkRecord> records = getRecords(containerType);
    auto it = find_if(records.begin(), records.end(), [id](const WorkRecord& r) { return r.getId() == id; });
    if (it == records.end()) { cout << "запись не найдена\n"; return; }
    vector<WorkRecord> updated;
    for (const WorkRecord& record : records)
        if (record.getId() != id) updated.push_back(record);
    if (!inputYesNo("удалить найденную запись?")) { cout << "удаление отменено\n"; return; }
    setRecords(containerType, updated);
    saveToFile(containerType);
    cout << "запись удалена\n";
}

void MedicalRegistry::editRecord(int containerType) {
    vector<WorkRecord> records = getRecords(containerType);
    if (records.empty()) { cout << "контейнер пуст\n"; return; }
    int id = inputInt("введите id записи для редактирования-> ", 1, 999999);
    auto it = find_if(records.begin(), records.end(), [id](const WorkRecord& r) { return r.getId() == id; });
    if (it == records.end()) { cout << "запись не найдена\n"; return; }
    while (true) {
        cout << "\nкакое поле изменить?\n"
            << "1. фио\n2. должность\n3. отделение\n4. дата\n5. количество часов\n0. завершить редактирование\n";
        int choice = inputInt("выберите поле-> ", 0, 5);
        if (choice == 0) break;
        if (choice == 1) it->setFullName(inputString("введите новое фио-> ", 60));
        else if (choice == 2) it->setPosition(inputString("введите новую должность-> ", 40));
        else if (choice == 3) it->setDepartment(inputString("введите новое отделение-> ", 40));
        else if (choice == 4) {
            int day, month, year;
            while (true) {
                day = inputInt("введите день-> ", 1, 31);
                month = inputInt("введите месяц-> ", 1, 12);
                year = inputInt("введите год (2000-2100)-> ", 2000, 2100);
                if (WorkRecord::isValidDate(day, month, year)) break;
                cout << "такой календарной даты не существует. повторите ввод\n";
            }
            it->setDate(day, month, year);
        }
        else if (choice == 5) it->setHours(inputDouble("введите новое количество часов (0.1-24.0)-> ", 0.1, 24.0));
        cout << "поле успешно изменено\n";
    }
    setRecords(containerType, records);
    saveToFile(containerType);
    cout << "редактирование завершено\n";
}

void MedicalRegistry::searchRecord(int containerType) const {
    vector<WorkRecord> records = getRecords(containerType);
    if (records.empty()) { cout << "контейнер пуст\n"; return; }
    int id = inputInt("введите id записи для поиска-> ", 1, 999999);
    vector<WorkRecord> found;
    for (const WorkRecord& record : records)
        if (record.getId() == id) found.push_back(record);
    if (found.empty()) cout << "запись не найдена\n";
    else printTable(found);
}

void MedicalRegistry::saveToFile(int containerType) const {
    const string fileName = getFileName(containerType);
    ofstream file(fileName, ios::trunc);
    if (!file.is_open()) {
        cout << "не удалось открыть файл " << fileName << " для сохранения данных\n";
        return;
    }

    vector<WorkRecord> records = getRecords(containerType);
    for (const WorkRecord& record : records) {
        file << record.getId() << '|'
            << escapeField(record.getFullName()) << '|'
            << escapeField(record.getPosition()) << '|'
            << escapeField(record.getDepartment()) << '|'
            << record.getDay() << '|' << record.getMonth() << '|'
            << record.getYear() << '|' << fixed << setprecision(1)
            << record.getHours() << '\n';
    }
    if (!file.good()) cout << "ошибка при записи данных в файл " << fileName << "\n";
    else cout << "данные сохранены в файл " << fileName << "\n";
}

void MedicalRegistry::sortRecords(int containerType) const {
    vector<WorkRecord> records = getRecords(containerType);
    if (records.empty()) { cout << "контейнер пуст\n"; return; }
    cout << "\nсортировка по полю:\n1. фио\n2. отделение\n3. дате\n4. количеству часов\n";
    int field = inputInt("выберите поле-> ", 1, 4);
    int direction = inputInt("порядок сортировки (1 - по возрастанию, 2 - по убыванию)-> ", 1, 2);
    sort(records.begin(), records.end(), [field, direction](const WorkRecord& a, const WorkRecord& b) {
        bool result = false;
        if (field == 1) result = a.getFullName() < b.getFullName();
        else if (field == 2) result = a.getDepartment() < b.getDepartment();
        else if (field == 3) result = a.getDateKey() < b.getDateKey();
        else result = a.getHours() < b.getHours();
        return direction == 1 ? result : !result && (
            field == 1 ? a.getFullName() != b.getFullName() :
            field == 2 ? a.getDepartment() != b.getDepartment() :
            field == 3 ? a.getDateKey() != b.getDateKey() : a.getHours() != b.getHours());
        });
    cout << "результат сортировки (порядок самого контейнера не изменяется):\n";
    printTable(records);
}

void MedicalRegistry::printRecords(int containerType) const {
    vector<WorkRecord> records = getRecords(containerType);
    if (containerType == 1) cout << "\nсодержимое queue (с начала очереди):\n";
    else if (containerType == 2) cout << "\nсодержимое stack (с вершины стека):\n";
    else cout << "\nсодержимое priority_queue (от наивысшего приоритета):\n";
    printTable(records);
}

void MedicalRegistry::run() {
    bool running = true;
    while (running) {
        cout << "\n---------- учет рабочего времени медицинских работников ----------\n"
            << "выберите контейнер:\n1. queue (очередь)\n2. stack (стек)\n"
            << "3. priority_queue (очередь с приоритетом)\n0. выход\n";
        int containerType = inputInt("ваш выбор-> ", 0, 3);
        if (containerType == 0) {
            if (inputYesNo("завершить работу программы?")) running = false;
            continue;
        }
        bool inMenu = true;
        while (inMenu) {
            cout << "\n---------------- меню контейнера ----------------\n"
                << "1. добавить запись\n2. удалить запись по id\n3. редактировать запись\n"
                << "4. вывести все записи\n5. поиск по id\n6. сортировка записей\n"
                << "7. сохранить записи в файл\n8. количество записей\n"
                << "0. вернуться к выбору контейнера\n";
            int choice = inputInt("выберите действие-> ", 0, 8);
            if (choice == 1) addRecord(containerType);
            else if (choice == 2) deleteRecord(containerType);
            else if (choice == 3) editRecord(containerType);
            else if (choice == 4) printRecords(containerType);
            else if (choice == 5) searchRecord(containerType);
            else if (choice == 6) sortRecords(containerType);
            else if (choice == 7) saveToFile(containerType);
            else if (choice == 8) cout << "количество записей: " << getRecords(containerType).size() << "\n";
            else inMenu = false;
            if (inMenu && inputYesNo("продолжить работу с выбранным контейнером?")) continue;
            else if (inMenu) inMenu = false;
        }
    }
    cout << "работа программы завершена\n";
}
