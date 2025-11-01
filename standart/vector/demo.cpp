#include "student_manager.h"

int main() {
    cout << "=== 1. Creating vector ===" << endl;
    vector<Student> students = readCSVToVector("students.csv");
    cout << "Students loaded: " << students.size() << endl << endl;

    cout << "=== 2. Changing student group ===" << endl;
    string emailToChange = "ivan.kravchenko@student.org";
    string newGroup = "NEW-GROUP-01";

    cout << "Before change:" << endl;
    for (const auto& student : students) {
        if (student.m_email == emailToChange) {
            student.print();
            break;
        }
    }

    if (changeGroupByEmail(students, emailToChange, newGroup)) {
        cout << "After change:" << endl;
        for (const auto& student : students) {
            if (student.m_email == emailToChange) {
                student.print();
                break;
            }
        }
    } else {
        cout << "Student not found!" << endl;
    }
    cout << endl;

    changeGroupByEmail(students, emailToChange, "NXE-49");

    cout << "=== 3. Group with most students ===" << endl;
    findGroupWithMostStudents(students);
    cout << endl;

    cout << "=== 4. Group with highest average rating ===" << endl;
    findGroupWithHighestRating(students);
    cout << endl;

    cout << "=== 5. Radix Sort by birthday ===" << endl;
    cout << "Before sorting:" << endl;
    for (size_t i = 0; i < min(size_t(5), students.size()); i++) {
        students[i].print();
    }

    radixSortByBirthday(students);

    cout << "\nAfter Radix Sort (by month and day):" << endl;
    for (size_t i = 0; i < min(size_t(5), students.size()); i++) {
        students[i].print();
    }

    saveToCSV(students, "students_sorted.csv");

    return 0;
}
