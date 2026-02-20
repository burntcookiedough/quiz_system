#pragma once
#include "../interfaces/IGradable.h"
#include "Quiz.h"
#include "Student.h"
#include <map>

class QuizAttempt : public IGradable {
private:
    std::string attemptId;
    std::shared_ptr<Student> student;
    std::shared_ptr<Quiz> quiz;
    std::map<std::string, std::string> studentAnswers; // QuestionId -> Answer String
    int score;
    bool submitted;

public:
    QuizAttempt(std::string id, std::shared_ptr<Student> s, std::shared_ptr<Quiz> q)
        : attemptId(std::move(id)), student(s), quiz(q), score(0), submitted(false) {}

    void recordAnswer(const std::string& questionId, const std::string& answer) {
        if (!submitted) {
            studentAnswers[questionId] = answer;
        }
    }

    void submitQuiz() {
        if (!submitted) {
            score = calculateScore();
            submitted = true;
        }
    }

    int calculateScore() const override {
        int tempScore = 0;
        const auto& questions = quiz->getQuestions();
        for (const auto& q : questions) {
            auto it = studentAnswers.find(q->getQuestionId());
            if (it != studentAnswers.end()) {
                if (q->checkAnswer(it->second)) {
                    tempScore += q->getMarks();
                }
            }
        }
        return tempScore;
    }

    std::string getAttemptId() const { return attemptId; }
    std::shared_ptr<Student> getStudent() const { return student; }
    std::shared_ptr<Quiz> getQuiz() const { return quiz; }
    int getScore() const { return score; }
    bool isSubmitted() const { return submitted; }
    const std::map<std::string, std::string>& getAnswers() const { return studentAnswers; }
};
