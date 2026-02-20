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
        std::string type = j.value("type", "");
        std::string id = j.value("id", "");
        std::string text = j.value("text", "");
        int marks = j.value("marks", 0);

        if (type == "MCQ") {
            auto options = j.contains("options") ? j["options"].get<std::vector<std::string>>() : std::vector<std::string>{};
            std::string correct = j.value("correctAnswer", j.value("correctOption", ""));
            return std::make_unique<MCQQuestion>(id, text, marks, options, correct);
        } else if (type == "TrueFalse") {
            bool correct = j.value("correctAnswer", false);
            return std::make_unique<TrueFalseQuestion>(id, text, marks, correct);
        } else if (type == "ShortAnswer") {
            std::string correct = j.value("correctAnswer", "");
            return std::make_unique<ShortAnswerQuestion>(id, text, marks, correct);
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
        std::string quizId = j.value("quizId", j.value("id", ""));
        std::string title = j.value("title", "Untitled");
        std::string authorId = j.value("authorId", "Unknown");

        auto quiz = std::make_unique<Quiz>(quizId, title, authorId);
        
        if (j.contains("questions") && j["questions"].is_array()) {
            for (const auto& qJson : j["questions"]) {
                auto q = jsonToQuestion(qJson);
                if (q) {
                    quiz->addQuestion(std::move(q));
                }
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
