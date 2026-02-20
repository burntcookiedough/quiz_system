/**
 * Structured Canvas - API Client
 * Wraps native fetch to centralize API calls to the C++ Backend.
 */

const API_BASE = 'http://localhost:8080/api';

const api = {
    // --- Teacher Endpoints ---

    getTeacherDashboard: async () => {
        const response = await fetch(`${API_BASE}/teacher/dashboard`);
        if (!response.ok) throw new Error('Failed to fetch dashboard data');
        return response.json();
    },

    createQuiz: async (quizData) => {
        const response = await fetch(`${API_BASE}/quizzes`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(quizData)
        });
        if (!response.ok) throw new Error('Failed to create quiz');
        return response.json();
    },

    // --- Student Endpoints ---

    getStudentDashboard: async (studentId) => {
        const response = await fetch(`${API_BASE}/student/dashboard/${studentId}`);
        if (!response.ok) throw new Error('Failed to fetch student data');
        return response.json();
    },

    getQuizContent: async (quizId, forTake = false) => {
        const fetchUrl = forTake ? `${API_BASE}/quizzes/${quizId}?take=true` : `${API_BASE}/quizzes/${quizId}`;
        const response = await fetch(fetchUrl);
        if (!response.ok) throw new Error('Failed to fetch quiz details');
        return response.json();
    },

    submitAttempt: async (quizId, attemptData) => {
        const response = await fetch(`${API_BASE}/attempts/${quizId}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(attemptData)
        });
        if (!response.ok) throw new Error('Failed to submit attempt');
        return response.json();
    }
};

window.StructAPI = api;
