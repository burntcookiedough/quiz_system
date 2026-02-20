#pragma once

class IGradable {
public:
    virtual ~IGradable() = default;
    virtual int calculateScore() const = 0;
};
