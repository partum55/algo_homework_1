#include "student_manager.h"
#include <chrono>

int main() {
    string filename = "students.csv";
    unordered_map<string, Student> studentMap = readCSVToHashTable(filename);

    // to vector
    vector<Student> students1, students2;
    for (const auto& pair : studentMap) {
        students1.push_back(pair.second);
    }
    students2 = students1;

    // Radix Sort
    auto start = chrono::high_resolution_clock::now();
    radixSort(students1);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedRadix = end - start;
    cout << "Radix Sort elapsed time: " << elapsedRadix.count() << " s" << endl;

    // Quick Sort
    start = chrono::high_resolution_clock::now();
    quickSort(students2, 0, students2.size() - 1);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedQuick = end - start;
    cout << "Quick Sort elapsed time: " << elapsedQuick.count() << " s" << endl;

    // correctness check
    bool same = true;
    for (size_t i = 0; i < students1.size(); i++) {
        if (students1[i].m_birth_month != students2[i].m_birth_month ||
            students1[i].m_birth_day != students2[i].m_birth_day) {
            same = false;
            break;
            }
    }
    cout << "Sorting results are " << (same ? "identical" : "different") << endl;

    return 0;
}
