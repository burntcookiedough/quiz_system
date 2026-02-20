#pragma once
#include <string>

class Question {
protected:
    std::string questionId;
    std::string questionText;
    int marks;

public:
    Question(std::string id, std::string text, int m) 
        : questionId(std::move(id)), questionText(std::move(text)), marks(m) {}

    virtual ~Question() = default;

    std::string getQuestionId() const { return questionId; }
    std::string getQuestionText() const { return questionText; }
    int getMarks() const { return marks; }

    virtual bool checkAnswer(const std::string& answer) const = 0;
    virtual std::string getType() const = 0;
};
