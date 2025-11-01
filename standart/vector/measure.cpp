#include "student_manager.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;
using namespace chrono;

int main() {
    vector<Student> students = readCSVToVector("students.csv");

    cout << "Loading students from CSV..." << endl;
    cout << "Loaded " << students.size() << " students" << endl << endl;

    vector<string> emails;
    vector<string> groups;

    for (const auto& student : students) {
        emails.push_back(student.m_email);
    }

    vector<pair<string, int>> groupSet;
    for (const auto& student : students) {
        bool found = false;
        for (auto& pair : groupSet) {
            if (pair.first == student.m_group) {
                pair.second++;
                found = true;
                break;
            }
        }
        if (!found) {
            groupSet.push_back({student.m_group, 1});
        }
    }

    for (const auto& pair : groupSet) {
        groups.push_back(pair.first);
    }

    if (emails.empty() || groups.empty()) {
        cerr << "No data loaded!" << endl;
        return 1;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> emailDist(0, emails.size() - 1);
    uniform_int_distribution<> groupDist(0, groups.size() - 1);
    uniform_int_distribution<> opDist(1, 106);

    cout << "Starting benchmark WITHOUT CACHE (10 seconds)..." << endl;
    cout << "Operation ratio - Op1:Op2:Op3 = 5:1:100" << endl << endl;

    int op1Count = 0, op2Count = 0, op3Count = 0;
    int totalOps = 0;

    auto startTime = high_resolution_clock::now();
    auto endTime = startTime + seconds(10);

    while (high_resolution_clock::now() < endTime) {
        int opType = opDist(gen);

        if (opType <= 5) {
            // Operation 1: Find group with most students
            vector<pair<string, int>> groupCount;
            for (const auto& student : students) {
                bool found = false;
                for (auto& pair : groupCount) {
                    if (pair.first == student.m_group) {
                        pair.second++;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    groupCount.push_back({student.m_group, 1});
                }
            }
            string maxGroup;
            int maxCount = 0;
            for (const auto& pair : groupCount) {
                if (pair.second > maxCount) {
                    maxCount = pair.second;
                    maxGroup = pair.first;
                }
            }
            op1Count++;
        } else if (opType == 6) {
            // Operation 2: Change student group
            string email = emails[emailDist(gen)];
            string newGroup = groups[groupDist(gen)];
            changeGroupByEmail(students, email, newGroup);
            op2Count++;
        } else {
            // Operation 3: Find group with highest rating
            vector<pair<string, GroupStats>> groupStats;
            for (const auto& student : students) {
                bool found = false;
                for (auto& pair : groupStats) {
                    if (pair.first == student.m_group) {
                        pair.second.studentCount++;
                        pair.second.totalRating += student.m_rating;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    GroupStats stats;
                    stats.studentCount = 1;
                    stats.totalRating = student.m_rating;
                    groupStats.push_back({student.m_group, stats});
                }
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
            op3Count++;
        }

        totalOps++;
    }

    auto actualEndTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(actualEndTime - startTime).count();

    cout << "=== BENCHMARK RESULTS (WITHOUT CACHE) ===" << endl;
    cout << "Duration: " << duration << " ms" << endl;
    cout << "Total operations: " << totalOps << endl;
    cout << "Operations per second: " << (totalOps * 1000.0 / duration) << endl << endl;

    cout << "Operation 1 (Find group with most students): " << op1Count << endl;
    cout << "Operation 2 (Change student group): " << op2Count << endl;
    cout << "Operation 3 (Find group with highest rating): " << op3Count << endl << endl;

    cout << "Actual ratio: " << op1Count << ":" << op2Count << ":" << op3Count << endl;

    double ratio1 = op2Count > 0 ? (double)op1Count / op2Count : 0;
    double ratio3 = op2Count > 0 ? (double)op3Count / op2Count : 0;
    cout << "Normalized ratio: " << fixed << setprecision(1)
         << ratio1 << ":1:" << ratio3 << endl;

    cout << "\n=== VERIFICATION ===" << endl;
    cout << "Group with most students: " << findGroupWithMostStudents(students) << endl;
    cout << "Group with highest rating: " << findGroupWithHighestRating(students) << endl;

    return 0;
}
