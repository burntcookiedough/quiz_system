#include <iostream>
#include <map>
#include <vector>
#include "../include/httplib.h"
#include "../include/json.hpp"
#include "../include/models/Quiz.h"
#include "../include/models/QuizAttempt.h"
#include "../include/models/Teacher.h"
#include "../include/models/Student.h"
#include "../include/models/MCQQuestion.h"
#include "../include/models/TrueFalseQuestion.h"
#include "../include/models/ShortAnswerQuestion.h"
#include "../include/services/FileManager.h"

using json = nlohmann::json;

// In-memory state for now
std::map<std::string, std::unique_ptr<Quiz>> quizzes;
std::map<std::string, std::unique_ptr<QuizAttempt>> attempts;

void setupInitialData() {
    // Create a dummy teacher
    Teacher t("U001", "Professor Smith");
    
    // Create a dummy quiz
    auto q1 = std::make_unique<Quiz>("Q001", "C++ Fundamentals", t.getUserId());
    
    auto mcq = std::make_unique<MCQQuestion>(
        "q_001_1", "What does OOP stand for?", 5, 
        std::vector<std::string>{"Object Oriented Programming", "Only Output Prints", "Out Of Place", "Overload Object Parameters"}, 
        "Object Oriented Programming");
        
    auto tf = std::make_unique<TrueFalseQuestion>(
        "q_001_2", "C++ supports multiple inheritance.", 2, true);
        
    auto sa = std::make_unique<ShortAnswerQuestion>(
        "q_001_3", "What memory management keyword deletes an object?", 3, "delete");

    q1->addQuestion(std::move(mcq));
    q1->addQuestion(std::move(tf));
    q1->addQuestion(std::move(sa));

    FileManager::saveQuiz(*q1, "data/Q001.json");
    quizzes["Q001"] = std::move(q1);
}

int main() {
    setupInitialData();

    httplib::Server svr;

    // Serve static files from the 'ui' directory
    svr.set_mount_point("/", "./ui");

    // Configure CORS so frontend can easily talk to api
    auto set_cors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    svr.Options(R"(/.*)", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        res.status = 200;
    });

    // API: Get all available quizzes (Summary)
    svr.Get("/api/quizzes", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        json response = json::array();
        for (const auto& pair : quizzes) {
            json q;
            q["id"] = pair.second->getQuizId();
            q["title"] = pair.second->getTitle();
            q["authorId"] = pair.second->getAuthorId();
            q["totalMarks"] = pair.second->getTotalMarks();
            response.push_back(q);
        }
        res.set_content(response.dump(), "application/json");
    });

    // API: Get specific quiz details
    svr.Get(R"(/api/quizzes/([a-zA-Z0-9_-]+))", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string quizId = req.matches[1];
        if (quizzes.find(quizId) != quizzes.end()) {
            json response = FileManager::quizToJson(*quizzes[quizId]);
            // For security, a real system might strip correct answers when sending to student.
            // For this project, we might just send the whole thing, or filter it.
            // Let's send the whole thing so the frontend can check, or let the backend check.
            res.set_content(response.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(R"({"error": "Quiz not found"})", "application/json");
        }
    });

    // API: Submit quiz attempt (Auto-grading)
    svr.Post(R"(/api/attempts/([a-zA-Z0-9_-]+))", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string quizId = req.matches[1];
        if (quizzes.find(quizId) != quizzes.end()) {
            try {
                json body = json::parse(req.body);
                std::string studentName = body.value("studentName", "Anonymous");
                auto student = std::make_shared<Student>("S_" + std::to_string(rand()), studentName);
                
                // Create attempt
                // Note: We need a shared_ptr for Quiz as well for the attempt,
                // But our map holds unique_ptr. To fix this quickly without changing models,
                // we'll just pull the data and calculate it directly here, or change the map.
                // For simplicity, let's just calculate the score right here.
                
                int score = 0;
                json results = json::object();
                
                for (const auto& ans : body["answers"]) {
                    std::string qId = ans["questionId"];
                    std::string sAns = ans["answer"]; // assuming everything comes in as literal string
                    
                    // find question
                    for(const auto& q : quizzes[quizId]->getQuestions()) {
                        if(q->getQuestionId() == qId) {
                            bool isCorrect = q->checkAnswer(sAns);
                            results[qId] = isCorrect;
                            if (isCorrect) score += q->getMarks();
                            break;
                        }
                    }
                }

                json response;
                response["score"] = score;
                response["total"] = quizzes[quizId]->getTotalMarks();
                response["details"] = results;

                res.set_content(response.dump(), "application/json");

            } catch (...) {
                res.status = 400;
                res.set_content(R"({"error": "Bad payload"})", "application/json");
            }
        } else {
            res.status = 404;
        }
    });
    
    // API: Teacher creates a new quiz
    svr.Post("/api/quizzes", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            json j = json::parse(req.body);
            auto newQuiz = FileManager::jsonToQuiz(j);
            if (newQuiz) {
                std::string id = newQuiz->getQuizId();
                quizzes[id] = std::move(newQuiz);
                FileManager::saveQuiz(*quizzes[id], "data/" + id + ".json");
                res.set_content(R"({"status":"success"})", "application/json");
            } else {
                res.status = 400;
                res.set_content(R"({"error":"Invalid quiz format"})", "application/json");
            }
        } catch(...) {
            res.status = 400;
        }
    });

    std::cout << "Starting Quiz API Server on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
