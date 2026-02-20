#pragma once
#include "User.h"
#include <iostream>

class Student : public User {
public:
    Student(std::string id, std::string name) 
        : User(std::move(id), std::move(name), "student") {}

    bool login(const std::string& password) override {
        // Simple stub for now
        std::cout << "Student " << name << " logged in." << std::endl;
        return true;
    }
};
