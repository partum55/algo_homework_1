#include "student_manager_o.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;
using namespace chrono;

int main() {
    OptimizedStudentDB db;

    cout << "Loading students from CSV..." << endl;
    db.loadFromCSV("students.csv");
    cout << "Loaded " << db.getStudentCount() << " students" << endl << endl;

    vector<string> emails = db.getAllEmails();
    vector<string> groups = db.getAllGroups();

    if (emails.empty() || groups.empty()) {
        cerr << "No data loaded!" << endl;
        return 1;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> emailDist(0, emails.size() - 1);
    uniform_int_distribution<> groupDist(0, groups.size() - 1);
    uniform_int_distribution<> opDist(1, 106);

    cout << "Starting benchmark (10 seconds)..." << endl;
    cout << "Operation ratio - Op1:Op2:Op3 = 5:1:100" << endl << endl;

    int op1Count = 0, op2Count = 0, op3Count = 0;
    int totalOps = 0;

    auto startTime = high_resolution_clock::now();
    auto endTime = startTime + seconds(10);

    while (high_resolution_clock::now() < endTime) {
        int opType = opDist(gen);

        if (opType <= 5) {
            db.findGroupWithMostStudents();
            op1Count++;
        } else if (opType == 6) {
            string email = emails[emailDist(gen)];
            string newGroup = groups[groupDist(gen)];
            db.changeGroupByEmail(email, newGroup);
            op2Count++;
        } else {
            db.findGroupWithHighestRating();
            op3Count++;
        }

        totalOps++;
    }

    auto actualEndTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(actualEndTime - startTime).count();

    cout << "=== BENCHMARK RESULTS ===" << endl;
    cout << "Duration: " << duration << " ms" << endl;
    cout << "Total operations: " << totalOps << endl;
    cout << "Operations per second: " << (totalOps * 1000.0 / duration) << endl << endl;

    cout << "Operation 1 (Find group with most students): " << op1Count << endl;
    cout << "Operation 2 (Change student group): " << op2Count << endl;
    cout << "Operation 3 (Find group with highest rating): " << op3Count << endl << endl;

    cout << "Actual ratio: " << op1Count << ":" << op2Count << ":" << op3Count << endl;

    double ratio1 = (double)op1Count / op2Count;
    double ratio3 = (double)op3Count / op2Count;
    cout << "Normalized ratio: " << fixed << setprecision(1)
         << ratio1 << ":1:" << ratio3 << endl;

    cout << "\n=== VERIFICATION ===" << endl;
    cout << "Group with most students: " << db.findGroupWithMostStudents() << endl;
    cout << "Group with highest rating: " << db.findGroupWithHighestRating() << endl;

    return 0;
}