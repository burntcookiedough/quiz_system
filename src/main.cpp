#include <iostream>
#include <map>
#include <vector>
#include <chrono>
#include <algorithm>
#include <filesystem>
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

struct SavedAttempt {
    std::string attemptId;
    std::string studentId;
    std::string studentName;
    std::string quizId;
    std::string quizTitle;
    int score;
    int total;
    long long timestamp; 
};
std::vector<SavedAttempt> globalAttempts;

long long getCurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void setupInitialData() {
    // Ensure data directory exists
    std::filesystem::create_directory("data");

    // Create a dummy teacher
    Teacher t("U001", "Professor Smith");
    
    // Create a dummy quiz
    auto q1 = std::make_unique<Quiz>("Q001", "Advanced Physics: Kinematics", t.getUserId());
    
    auto mcq = std::make_unique<MCQQuestion>(
        "q_001_1", "A car accelerates uniformly from rest to a speed of 25 m/s in 8.0 seconds. What is the acceleration?", 10, 
        std::vector<std::string>{"2.5 m/s²", "3.1 m/s²", "4.0 m/s²", "200 m/s²"}, 
        "3.1 m/s²");
        
    auto sa = std::make_unique<ShortAnswerQuestion>(
        "q_001_2", "Define Newton's First Law of Motion and provide a real-world example.", 15, "inertia");

    q1->addQuestion(std::move(mcq));
    q1->addQuestion(std::move(sa));

    FileManager::saveQuiz(*q1, "data/Q001.json");
    quizzes["Q001"] = std::move(q1);

    // Add some dummy attempts to populate the dashboards
    long long now = getCurrentTimeMs();
    long long dayMs = 24LL * 60 * 60 * 1000;

    globalAttempts.push_back({"A001", "student_dummy", "L. Crain", "Q002", "Thermodynamics Quiz", 92, 100, now - 3*dayMs});
    globalAttempts.push_back({"A002", "S002", "Eleanor Vance", "Q003", "Module 4: Thermodynamics", 92, 100, now - 1*dayMs});
    globalAttempts.push_back({"A003", "student_dummy", "Luke Crain", "Q003", "Module 4: Thermodynamics", 71, 100, now - 2*dayMs});
    globalAttempts.push_back({"A004", "S003", "Theo Crain", "Q004", "Midterm Preparation", 88, 100, now - 4*dayMs});
    globalAttempts.push_back({"A005", "S004", "Shirley Jackson", "Q001", "Advanced Physics: Kinematics", 25, 25, now - 6*dayMs});
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
            
            if (req.has_param("take") && req.get_param_value("take") == "true") {
                for (auto& q : response["questions"]) {
                    q.erase("correctAnswer");
                }
            }

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
                std::string studentId = body.value("studentId", "student_dummy");
                
                int score = 0;
                json results = json::object();
                
                for (auto ans : body["answers"]) {
                    std::string qId = ans.value("questionId", "");
                    std::string sAns = ans.value("answer", ""); 
                    
                    for(const auto& q : quizzes[quizId]->getQuestions()) {
                        if(q->getQuestionId() == qId) {
                            bool isCorrect = q->checkAnswer(sAns);
                            results[qId] = isCorrect;
                            if (isCorrect) score += q->getMarks();
                            break;
                        }
                    }
                }

                int totalMarks = quizzes[quizId]->getTotalMarks();
                globalAttempts.push_back({
                    "A_" + std::to_string(getCurrentTimeMs()),
                    studentId,
                    studentName,
                    quizId,
                    quizzes[quizId]->getTitle(),
                    score,
                    totalMarks,
                    getCurrentTimeMs()
                });

                json response;
                response["score"] = score;
                response["total"] = totalMarks;
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

    // API: Teacher Dashboard Metrics
    svr.Get("/api/teacher/dashboard", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        json response;
        response["activeAssignments"] = quizzes.size();
        
        double totalScorePct = 0;
        int count = 0;
        json recent = json::array();

        auto sortedAttempts = globalAttempts;
        std::sort(sortedAttempts.begin(), sortedAttempts.end(), [](const SavedAttempt& a, const SavedAttempt& b) {
            return a.timestamp > b.timestamp;
        });

        for (const auto& a : sortedAttempts) {
            if (a.total > 0) {
                double pct = (double)a.score / a.total * 100.0;
                totalScorePct += pct;
                count++;
            }
            if (recent.size() < 4) {
                json r;
                r["studentName"] = a.studentName;
                r["quizTitle"] = a.quizTitle;
                r["score"] = (a.total > 0) ? ((double)a.score / a.total * 100.0) : 0;
                recent.push_back(r);
            }
        }

        response["avgClassScore"] = count > 0 ? (totalScorePct / count) : 0;
        response["needsGrading"] = 12; // Static for demo
        response["recentSubmissions"] = recent;

        std::vector<int> velocity(7, 0);
        long long now = getCurrentTimeMs();
        long long dayMs = 24LL * 60 * 60 * 1000;
        
        for (const auto& a : globalAttempts) {
            long long diff = now - a.timestamp;
            int daysAgo = diff / dayMs;
            if (daysAgo >= 0 && daysAgo < 7) {
                velocity[6 - daysAgo]++;
            }
        }
        
        json vArr = json::array();
        for (int v : velocity) vArr.push_back(v);
        response["velocityHistory"] = vArr;

        res.set_content(response.dump(), "application/json");
    });

    // API: Student Dashboard Metrics
    svr.Get(R"(/api/student/dashboard/([a-zA-Z0-9_-]+))", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        std::string studentId = req.matches[1];
        
        json response;
        
        json completed = json::array();
        std::vector<std::string> completedQuizIds;
        for (const auto& a : globalAttempts) {
            if (a.studentId == studentId) {
                json c;
                c["quizId"] = a.quizId;
                c["quizTitle"] = a.quizTitle;
                c["score"] = a.score;
                c["total"] = a.total;
                completed.push_back(c);
                completedQuizIds.push_back(a.quizId);
            }
        }

        json active = json::array();
        for (const auto& pair : quizzes) {
            if (std::find(completedQuizIds.begin(), completedQuizIds.end(), pair.first) == completedQuizIds.end()) {
                json act;
                act["id"] = pair.first;
                act["title"] = pair.second->getTitle();
                act["totalMarks"] = pair.second->getTotalMarks();
                active.push_back(act);
            }
        }

        response["completedAssignments"] = completed;
        response["activeAssignments"] = active;

        res.set_content(response.dump(), "application/json");
    });

    std::cout << "Starting Quiz API Server on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
