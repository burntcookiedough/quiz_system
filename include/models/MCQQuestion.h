#pragma once
#include "Question.h"
#include <vector>

class MCQQuestion : public Question {
private:
    std::vector<std::string> options;
    std::string correctOption;

public:
    MCQQuestion(std::string id, std::string text, int m, std::vector<std::string> opts, std::string correct) 
        : Question(std::move(id), std::move(text), m), options(std::move(opts)), correctOption(std::move(correct)) {}

    bool checkAnswer(const std::string& answer) const override {
        return answer == correctOption;
    }

    std::string getType() const override { return "MCQ"; }

    const std::vector<std::string>& getOptions() const { return options; }
    std::string getCorrectOption() const { return correctOption; }
};
