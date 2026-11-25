#pragma once

#include <string>

using namespace std;

class WorkRecord {
private:
    int id;
    string fullName;
    string position;
    string department;
    int day;
    int month;
    int year;
    double hours;

public:
    static const double NORM_HOURS;

    WorkRecord();
    WorkRecord(int id, const string& fullName, const string& position,
        const string& department, int day, int month, int year, double hours);

    int getId() const;
    const string& getFullName() const;
    const string& getPosition() const;
    const string& getDepartment() const;
    int getDay() const;
    int getMonth() const;
    int getYear() const;
    double getHours() const;
    double getOvertime() const;
    string getDateString() const;
    int getDateKey() const;

    void setFullName(const string& value);
    void setPosition(const string& value);
    void setDepartment(const string& value);
    void setDate(int day, int month, int year);
    void setHours(double value);

    static bool isValidDate(int day, int month, int year);
};

bool operator<(const WorkRecord& first, const WorkRecord& second);

struct PriorityCompare {
    bool operator()(const WorkRecord& first, const WorkRecord& second) const;
};
