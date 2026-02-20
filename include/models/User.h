#pragma once
#include <string>

class User {
protected:
    std::string userId;
    std::string name;
    std::string role;

public:
    User(std::string id, std::string name, std::string role) 
        : userId(std::move(id)), name(std::move(name)), role(std::move(role)) {}
    
    virtual ~User() = default;

    // Getters
    std::string getUserId() const { return userId; }
    std::string getName() const { return name; }
    std::string getRole() const { return role; }

    // Setters
    void setName(const std::string& newName) { name = newName; }

    // Authenticators
    virtual bool login(const std::string& password) = 0; // Pure virtual, implemented differently maybe? For now just stub
};
