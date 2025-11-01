#include "student_manager.h"

int main() {
    cout << "=== 1. Creating AVL tree ===" << endl;
    AVLTree students = readCSVToAVL("students.csv");
    cout << "Students loaded: " << students.size() << endl << endl;

    cout << "=== 2. Changing student group ===" << endl;
    string emailToChange = "ivan.kravchenko@student.org";
    string newGroup = "NEW-GROUP-01";

    cout << "Before change:" << endl;
    Student* student = students.search(emailToChange);
    if (student) {
        student->print();
    }

    if (changeGroupByEmail(students, emailToChange, newGroup)) {
        cout << "After change:" << endl;
        student = students.search(emailToChange);
        if (student) {
            student->print();
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
    vector<Student> studentsVec = students.getAllStudents();

    cout << "Before sorting:" << endl;
    for (size_t i = 0; i < min(size_t(5), studentsVec.size()); i++) {
        studentsVec[i].print();
    }

    radixSortByBirthday(studentsVec);

    cout << "\nAfter Radix Sort (by month and day):" << endl;
    for (size_t i = 0; i < min(size_t(5), studentsVec.size()); i++) {
        studentsVec[i].print();
    }

    saveToCSV(studentsVec, "students_sorted.csv");

    return 0;
}
