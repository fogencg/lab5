#pragma once

#include "WorkRecord.h"
#include <queue>
#include <stack>
#include <vector>
#include <string>

using namespace std;

class MedicalRegistry {
private:
    queue<WorkRecord> workQueue;
    stack<WorkRecord> workStack;
    priority_queue<WorkRecord, vector<WorkRecord>, PriorityCompare> priorityWorkQueue;

    vector<WorkRecord> getRecords(int containerType) const;
    void setRecords(int containerType, const vector<WorkRecord>& records);
    bool containsId(int containerType, int id) const;
    void printTable(const vector<WorkRecord>& records) const;
    void addRecord(int containerType);
    void deleteRecord(int containerType);
    void editRecord(int containerType);
    void searchRecord(int containerType) const;
    string getFileName(int containerType) const;
    void saveToFile(int containerType) const;
    void loadFromFile(int containerType);
    void sortRecords(int containerType) const;
    void printRecords(int containerType) const;

public:
    MedicalRegistry();
    void run();
};
