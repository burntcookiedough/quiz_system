#pragma once
#include "../json.hpp"
#include "../models/Quiz.h"
#include "../models/MCQQuestion.h"
#include "../models/TrueFalseQuestion.h"
#include "../models/ShortAnswerQuestion.h"
#include <fstream>
#include <memory>
#include <iostream>

using json = nlohmann::json;

class FileManager {
public:
    static json questionToJson(const Question* q) {
        json j;
        j["id"] = q->getQuestionId();
        j["text"] = q->getQuestionText();
        j["marks"] = q->getMarks();
        j["type"] = q->getType();

        if (q->getType() == "MCQ") {
            auto mcq = dynamic_cast<const MCQQuestion*>(q);
            j["options"] = mcq->getOptions();
            j["correctOption"] = mcq->getCorrectOption();
        } else if (q->getType() == "TrueFalse") {
            auto tf = dynamic_cast<const TrueFalseQuestion*>(q);
            j["correctAnswer"] = tf->getCorrectAnswer();
        } else if (q->getType() == "ShortAnswer") {
            auto sa = dynamic_cast<const ShortAnswerQuestion*>(q);
            j["correctAnswer"] = sa->getCorrectAnswer();
        }
        return j;
    }

    static std::unique_ptr<Question> jsonToQuestion(const json& j) {
        std::string type = j["type"];
        std::string id = j["id"];
        std::string text = j["text"];
        int marks = j["marks"];

        if (type == "MCQ") {
            return std::make_unique<MCQQuestion>(id, text, marks, j["options"].get<std::vector<std::string>>(), j["correctOption"]);
        } else if (type == "TrueFalse") {
            return std::make_unique<TrueFalseQuestion>(id, text, marks, j["correctAnswer"].get<bool>());
        } else if (type == "ShortAnswer") {
            return std::make_unique<ShortAnswerQuestion>(id, text, marks, j["correctAnswer"]);
        }
        return nullptr;
    }

    static json quizToJson(const Quiz& quiz) {
        json j;
        j["quizId"] = quiz.getQuizId();
        j["title"] = quiz.getTitle();
        j["authorId"] = quiz.getAuthorId();
        
        json questionsArray = json::array();
        for (const auto& q : quiz.getQuestions()) {
            questionsArray.push_back(questionToJson(q.get()));
        }
        j["questions"] = questionsArray;
        return j;
    }

    static std::unique_ptr<Quiz> jsonToQuiz(const json& j) {
        auto quiz = std::make_unique<Quiz>(j["quizId"], j["title"], j["authorId"]);
        for (const auto& qJson : j["questions"]) {
            auto q = jsonToQuestion(qJson);
            if (q) {
                quiz->addQuestion(std::move(q));
            }
        }
        return quiz;
    }

    static bool saveQuiz(const Quiz& quiz, const std::string& filepath) {
        try {
            json j = quizToJson(quiz);
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << j.dump(4);
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error saving quiz: " << e.what() << std::endl;
        }
        return false;
    }

    static std::unique_ptr<Quiz> loadQuiz(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (file.is_open()) {
                json j;
                file >> j;
                return jsonToQuiz(j);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading quiz: " << e.what() << std::endl;
        }
        return nullptr;
    }
};
