#pragma once
#include "Question.h"

class ShortAnswerQuestion : public Question {
private:
    std::string correctAnswer;

public:
    ShortAnswerQuestion(std::string id, std::string text, int m, std::string correct) 
        : Question(std::move(id), std::move(text), m), correctAnswer(std::move(correct)) {}

    bool checkAnswer(const std::string& answer) const override {
        return answer == correctAnswer;
    }

    std::string getType() const override { return "ShortAnswer"; }

    std::string getCorrectAnswer() const { return correctAnswer; }
};
