#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../models/Question.h"

class Quiz {
private:
    std::string quizId;
    std::string title;
    std::string authorId; // Teacher's ID
    std::vector<std::unique_ptr<Question>> questions;

public:
    Quiz(std::string id, std::string t, std::string author) 
        : quizId(std::move(id)), title(std::move(t)), authorId(std::move(author)) {}

    void addQuestion(std::unique_ptr<Question> q) {
        questions.push_back(std::move(q));
    }

    std::string getQuizId() const { return quizId; }
    std::string getTitle() const { return title; }
    std::string getAuthorId() const { return authorId; }

    int getTotalMarks() const {
        int total = 0;
        for (const auto& q : questions) {
            total += q->getMarks();
        }
        return total;
    }

    const std::vector<std::unique_ptr<Question>>& getQuestions() const {
        return questions;
    }
};
