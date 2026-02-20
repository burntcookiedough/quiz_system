#pragma once
#include "Question.h"

class TrueFalseQuestion : public Question {
private:
    bool correctAnswer;

public:
    TrueFalseQuestion(std::string id, std::string text, int m, bool correct) 
        : Question(std::move(id), std::move(text), m), correctAnswer(correct) {}

    bool checkAnswer(const std::string& answer) const override {
        // Assume answer string is "true" or "false" (case insensitive conceptually, but here exact match for simplicity)
        std::string expected = correctAnswer ? "true" : "false";
        return answer == expected;
    }

    std::string getType() const override { return "TrueFalse"; }

    bool getCorrectAnswer() const { return correctAnswer; }
};
