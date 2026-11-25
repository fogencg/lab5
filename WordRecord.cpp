#include "WorkRecord.h"
#include <iomanip>
#include <sstream>

const double WorkRecord::NORM_HOURS = 8.0;

WorkRecord::WorkRecord()
    : id(0), fullName(""), position(""), department(""),
    day(1), month(1), year(2000), hours(0.0) {}

WorkRecord::WorkRecord(int id, const string& fullName, const string& position,
    const string& department, int day, int month, int year, double hours)
    : id(id), fullName(fullName), position(position), department(department),
    day(day), month(month), year(year), hours(hours) {}

int WorkRecord::getId() const { return id; }
const string& WorkRecord::getFullName() const { return fullName; }
const string& WorkRecord::getPosition() const { return position; }
const string& WorkRecord::getDepartment() const { return department; }
int WorkRecord::getDay() const { return day; }
int WorkRecord::getMonth() const { return month; }
int WorkRecord::getYear() const { return year; }
double WorkRecord::getHours() const { return hours; }
double WorkRecord::getOvertime() const { return hours > NORM_HOURS ? hours - NORM_HOURS : 0.0; }

string WorkRecord::getDateString() const {
    ostringstream out;
    out << setfill('0') << setw(2) << day << '.'
        << setw(2) << month << '.' << setw(4) << year;
    return out.str();
}

int WorkRecord::getDateKey() const {
    return year * 10000 + month * 100 + day;
}

void WorkRecord::setFullName(const string& value) { fullName = value; }
void WorkRecord::setPosition(const string& value) { position = value; }
void WorkRecord::setDepartment(const string& value) { department = value; }
void WorkRecord::setDate(int d, int m, int y) { day = d; month = m; year = y; }
void WorkRecord::setHours(double value) { hours = value; }

bool WorkRecord::isValidDate(int day, int month, int year) {
    if (year < 2000 || year > 2100 || month < 1 || month > 12) return false;
    int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    bool leapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && leapYear) maxDay = 29;
    return day >= 1 && day <= maxDay;
}

bool operator<(const WorkRecord& first, const WorkRecord& second) {
    if (first.getFullName() != second.getFullName())
        return first.getFullName() < second.getFullName();
    return first.getDateKey() < second.getDateKey();
}


bool PriorityCompare::operator()(const WorkRecord& first, const WorkRecord& second) const {
    if (first.getHours() != second.getHours())
        return first.getHours() < second.getHours();
    return first.getDateKey() > second.getDateKey();
}
