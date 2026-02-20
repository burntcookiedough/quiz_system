#pragma once
#include "User.h"
#include <iostream>

class Teacher : public User {
public:
    Teacher(std::string id, std::string name) 
        : User(std::move(id), std::move(name), "teacher") {}

    bool login(const std::string& password) override {
        // Simple stub for now
        std::cout << "Teacher " << name << " logged in." << std::endl;
        return true;
    }
};
