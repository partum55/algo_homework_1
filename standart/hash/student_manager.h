#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Student {
    string m_name;
    string m_surname;
    string m_email;
    int m_birth_year;
    int m_birth_month;
    int m_birth_day;
    string m_group;
    double m_rating;
    string m_phone_number;

    void print() const {
        cout << m_name << " " << m_surname << " | " << m_group
             << " | " << m_birth_day << "." << m_birth_month
             << " | Rating: " << m_rating << endl;
    }
};

struct GroupStats {
    int studentCount = 0;
    double totalRating = 0.0;

    double getAverageRating() const {
        return studentCount > 0 ? totalRating / studentCount : 0.0;
    }
};

vector<string> parseCSVLine(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string field;

    while (getline(ss, field, ',')) {
        result.push_back(field);
    }
    return result;
}

unordered_map<string, Student> readCSVToHashTable(const string& filename) {
    unordered_map<string, Student> hashTable;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return hashTable;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        vector<string> fields = parseCSVLine(line);

        if (fields.size() >= 9) {
            Student student;
            student.m_name = fields[0];
            student.m_surname = fields[1];
            student.m_email = fields[2];
            student.m_birth_year = stoi(fields[3]);
            student.m_birth_month = stoi(fields[4]);
            student.m_birth_day = stoi(fields[5]);
            student.m_group = fields[6];
            student.m_rating = stod(fields[7]);
            student.m_phone_number = fields[8];

            hashTable[student.m_email] = student;
        }
    }

    file.close();
    return hashTable;
}

bool changeGroupByEmail(unordered_map<string, Student>& students,
                        const string& email, const string& newGroup) {
    auto it = students.find(email);
    if (it != students.end()) {
        it->second.m_group = newGroup;
        return true;
    }
    return false;
}

string findGroupWithMostStudents(const unordered_map<string, Student>& students) {
    unordered_map<string, int> groupCount;

    for (const auto& pair : students) {
        groupCount[pair.second.m_group]++;
    }

    string maxGroup;
    int maxCount = 0;

    for (const auto& pair : groupCount) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            maxGroup = pair.first;
        }
    }

    cout << "Group " << maxGroup << " has the most students: " << maxCount << endl;
    return maxGroup;
}

string findGroupWithHighestRating(const unordered_map<string, Student>& students) {
    unordered_map<string, GroupStats> groupStats;

    for (const auto& pair : students) {
        const Student& student = pair.second;
        groupStats[student.m_group].studentCount++;
        groupStats[student.m_group].totalRating += student.m_rating;
    }

    string bestGroup;
    double maxAvgRating = 0.0;

    for (const auto& pair : groupStats) {
        double avgRating = pair.second.getAverageRating();
        if (avgRating > maxAvgRating) {
            maxAvgRating = avgRating;
            bestGroup = pair.first;
        }
    }

    cout << "Group " << bestGroup << " has the highest average rating: "
         << fixed << setprecision(2) << maxAvgRating << endl;
    return bestGroup;
}

void radixSort(vector<Student>& students) {
    int n = students.size();
    if (n == 0) return;

    vector<Student> temp(n);

    {
        vector<vector<Student>> buckets(32);

        for (const auto& student : students) {
            buckets[student.m_birth_day].push_back(student);
        }

        int index = 0;
        for (int i = 1; i <= 31; i++) {
            for (const auto& student : buckets[i]) {
                students[index++] = student;
            }
        }
    }

    {
        vector<vector<Student>> buckets(13);

        for (const auto& student : students) {
            buckets[student.m_birth_month].push_back(student);
        }

        int index = 0;
        for (int i = 1; i <= 12; i++) {
            for (const auto& student : buckets[i]) {
                students[index++] = student;
            }
        }
    }
}

void saveToCSV(const vector<Student>& students, const string& filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error creating file!" << endl;
        return;
    }

    file << "m_name,m_surname,m_email,m_birth_year,m_birth_month,m_birth_day,m_group,m_rating,m_phone_number\n";

    for (const auto& s : students) {
        file << s.m_name << "," << s.m_surname << "," << s.m_email << ","
             << s.m_birth_year << "," << s.m_birth_month << "," << s.m_birth_day << ","
             << s.m_group << "," << fixed << setprecision(2) << s.m_rating << ","
             << s.m_phone_number << "\n";
    }

    file.close();
    cout << "\nFile saved: " << filename << endl;
}

bool compare(const Student& a, const Student& b) {
    if (a.m_birth_month == b.m_birth_month) {
        return a.m_birth_day < b.m_birth_day;
    }
    return a.m_birth_month < b.m_birth_month;
}

int partition(vector<Student>& students, int low, int high) {
    Student pivot = students[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compare(students[j], pivot)) {
            i++;
            swap(students[i], students[j]);
        }
    }
    swap(students[i + 1], students[high]);
    return i + 1;
}

void quickSort(vector<Student>& students, int low, int high) {
    if (low < high) {
        int pi = partition(students, low, high);
        quickSort(students, low, pi - 1);
        quickSort(students, pi + 1, high);
    }
}

#endif // STUDENT_MANAGER_H
