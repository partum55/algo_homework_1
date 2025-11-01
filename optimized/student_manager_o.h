#ifndef STUDENT_MANAGER_OPTIMIZED
#define STUDENT_MANAGER_OPTIMIZED

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <random>

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
};

struct GroupStats {
    int studentCount = 0;
    double totalRating = 0.0;
    
    double getAverageRating() const {
        return studentCount > 0 ? totalRating / studentCount : 0.0;
    }
};

class OptimizedStudentDB {
private:
    unordered_map<string, Student> studentsByEmail;
    unordered_map<string, GroupStats> groupStats;
    
    string cachedMaxCountGroup;
    int cachedMaxCount = 0;
    string cachedMaxRatingGroup;
    double cachedMaxRating = 0.0;
    bool cacheValid = false;

public:
    void loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file!" << endl;
            return;
        }
        
        string line;
        getline(file, line);
        
        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields;
            
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }
            
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
                
                studentsByEmail[student.m_email] = student;
                
                groupStats[student.m_group].studentCount++;
                groupStats[student.m_group].totalRating += student.m_rating;
            }
        }
        
        file.close();
        rebuildCache();
    }
    
    void rebuildCache() {
        cachedMaxCount = 0;
        cachedMaxRating = 0.0;
        
        for (const auto& pair : groupStats) {
            if (pair.second.studentCount > cachedMaxCount) {
                cachedMaxCount = pair.second.studentCount;
                cachedMaxCountGroup = pair.first;
            }
            
            double avgRating = pair.second.getAverageRating();
            if (avgRating > cachedMaxRating) {
                cachedMaxRating = avgRating;
                cachedMaxRatingGroup = pair.first;
            }
        }
        
        cacheValid = true;
    }
    
    string findGroupWithMostStudents() {
        if (!cacheValid) rebuildCache();
        return cachedMaxCountGroup;
    }
    
    bool changeGroupByEmail(const string& email, const string& newGroup) {
        auto it = studentsByEmail.find(email);
        if (it == studentsByEmail.end()) {
            return false;
        }
        
        Student& student = it->second;
        string oldGroup = student.m_group;
        
        if (oldGroup == newGroup) {
            return true;
        }
        
        groupStats[oldGroup].studentCount--;
        groupStats[oldGroup].totalRating -= student.m_rating;
        
        if (groupStats[oldGroup].studentCount == 0) {
            groupStats.erase(oldGroup);
        }
        
        student.m_group = newGroup;
        
        groupStats[newGroup].studentCount++;
        groupStats[newGroup].totalRating += student.m_rating;
        
        cacheValid = false;
        
        return true;
    }
    
    string findGroupWithHighestRating() {
        if (!cacheValid) rebuildCache();
        return cachedMaxRatingGroup;
    }
    
    int getStudentCount() const {
        return studentsByEmail.size();
    }
    
    vector<string> getAllEmails() const {
        vector<string> emails;
        emails.reserve(studentsByEmail.size());
        for (const auto& pair : studentsByEmail) {
            emails.push_back(pair.first);
        }
        return emails;
    }
    
    vector<string> getAllGroups() const {
        vector<string> groups;
        groups.reserve(groupStats.size());
        for (const auto& pair : groupStats) {
            groups.push_back(pair.first);
        }
        return groups;
    }
};

#endif // STUDENT_MANAGER_OPTIMIZED
