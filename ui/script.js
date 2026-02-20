const API_BASE = "http://localhost:8080/api";

// --- Navigation Flow ---
document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        switchTab(btn.dataset.target);
    });
});

function switchTab(targetId) {
    document.querySelectorAll('.view-section').forEach(sec => sec.classList.remove('active'));
    document.getElementById(targetId).classList.add('active');

    // Sync active state on buttons if called programmatically
    document.querySelectorAll('.tab-btn').forEach(b => {
        if(b.dataset.target === targetId) b.classList.add('active');
        else b.classList.remove('active');
    });

    if (targetId === 'student-view') {
        loadQuizzes();
        backToStudentDashboard(); // Reset student view
    }
}

// --- Toast Notification ---
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.textContent = message;
    container.appendChild(toast);
    setTimeout(() => {
        toast.style.opacity = '0';
        setTimeout(() => toast.remove(), 300);
    }, 4000);
}

// --- Teacher View Logic (Creating Quizzes) ---
let questionCounter = 0;

function addQuestionUI(type) {
    questionCounter++;
    const list = document.getElementById('questions-list');
    const qId = `q_${Date.now()}_${questionCounter}`;
    
    let innerHTML = '';
    
    if (type === 'MCQ') {
        innerHTML = `
            <div class="input-group mb">
                <label>Options (comma separated)</label>
                <input type="text" class="q-options" placeholder="A, B, C, D">
            </div>
            <div class="input-group">
                <label>Exact Correct Option</label>
                <input type="text" class="q-correct" placeholder="A">
            </div>
        `;
    } else if (type === 'TrueFalse') {
        innerHTML = `
            <div class="input-group">
                <label>Correct Answer</label>
                <select class="q-correct" style="padding:0.75rem; background:#0d1117; color:#fff; border:1px solid #30363d; border-radius:12px;">
                    <option value="true">True</option>
                    <option value="false">False</option>
                </select>
            </div>
        `;
    } else if (type === 'ShortAnswer') {
        innerHTML = `
            <div class="input-group">
                <label>Exact Correct String</label>
                <input type="text" class="q-correct" placeholder="Your Answer">
            </div>
        `;
    }

    const qBlock = document.createElement('div');
    qBlock.className = 'q-block fade-in-up';
    qBlock.dataset.type = type;
    qBlock.innerHTML = `
        <div class="q-header">
            <h4>${type} Question</h4>
            <button class="remove-btn" onclick="this.parentElement.parentElement.remove()">✕</button>
        </div>
        <div class="form-container">
            <div class="input-group" style="flex:2">
                <label>Question Text</label>
                <input type="text" class="q-text" placeholder="Enter question..." required>
            </div>
            <div class="input-group">
                <label>Marks</label>
                <input type="number" class="q-marks" value="1" min="1" required>
            </div>
        </div>
        <div class="specifics-container form-container" style="flex-direction: column; gap:1rem;">
            ${innerHTML}
        </div>
    `;
    list.appendChild(qBlock);
}

document.getElementById('save-quiz-btn').addEventListener('click', async () => {
    const title = document.getElementById('quiz-title').value;
    const qId = document.getElementById('quiz-id').value;
    
    if (!title || !qId) return showToast('Quiz Title and ID are required!', 'error');

    const questions = [];
    const blocks = document.querySelectorAll('.q-block');
    
    if(blocks.length === 0) return showToast('Add at least one question!', 'error');

    blocks.forEach((block, idx) => {
        const type = block.dataset.type;
        const text = block.querySelector('.q-text').value;
        const marks = parseInt(block.querySelector('.q-marks').value);
        
        const qObj = {
            id: `q_${qId.toLowerCase()}_${idx+1}`,
            type: type,
            text: text,
            marks: marks
        };

        if (type === 'MCQ') {
            qObj.options = block.querySelector('.q-options').value.split(',').map(s=>s.trim());
            qObj.correctOption = block.querySelector('.q-correct').value.trim();
        } else if (type === 'TrueFalse') {
            qObj.correctAnswer = block.querySelector('.q-correct').value === 'true';
        } else if (type === 'ShortAnswer') {
            qObj.correctAnswer = block.querySelector('.q-correct').value.trim();
        }
        
        questions.push(qObj);
    });

    const payload = {
        quizId: qId,
        title: title,
        authorId: "Teacher_1",
        questions: questions
    };

    try {
        const res = await fetch(`${API_BASE}/quizzes`, {
            method: 'POST',
            body: JSON.stringify(payload)
        });
        if(res.ok) {
            showToast('Quiz published successfully!');
            document.getElementById('questions-list').innerHTML = ''; // Clear
            document.getElementById('quiz-title').value = '';
            document.getElementById('quiz-id').value = '';
        } else {
            showToast('Failed to publish quiz', 'error');
        }
    } catch(err) {
        showToast('API Connection Error', 'error');
    }
});

// --- Student View Logic (Fetching & Attempting) ---
async function loadQuizzes() {
    const grid = document.getElementById('available-quizzes');
    grid.innerHTML = '<p>Loading quizzes...</p>';
    
    try {
        const res = await fetch(`${API_BASE}/quizzes`);
        if(!res.ok) throw new Error();
        const quizzes = await res.json();
        
        grid.innerHTML = '';
        if (quizzes.length === 0) {
            grid.innerHTML = '<p>No quizzes available right now.</p>';
            return;
        }

        quizzes.forEach(q => {
            grid.innerHTML += `
                <div class="card interactive available-quiz-card fade-in-up" onclick="startQuiz('${q.id}')">
                    <h3>${q.title}</h3>
                    <div class="meta">
                        <span>🏷️ ${q.id}</span>
                        <span>⭐ ${q.totalMarks} Marks Obj</span>
                    </div>
                    <button class="secondary-btn" style="width:100%">Attempt Now</button>
                </div>
            `;
        });
    } catch (err) {
        grid.innerHTML = '<p style="color:var(--danger)">Failed to connect to backend Server.</p>';
    }
}

let currentAttemptQuizId = null;

async function startQuiz(quizId) {
    document.getElementById('student-dashboard').classList.add('hidden');
    document.getElementById('quiz-results-view').classList.add('hidden');
    document.getElementById('quiz-taking-view').classList.remove('hidden');
    
    currentAttemptQuizId = quizId;
    document.getElementById('attempt-questions').innerHTML = '<p>Loading questions...</p>';

    try {
        const res = await fetch(`${API_BASE}/quizzes/${quizId}`);
        const quiz = await res.json();
        
        document.getElementById('attempt-quiz-title').textContent = quiz.title;
        
        const qContainer = document.getElementById('attempt-questions');
        qContainer.innerHTML = '';

        quiz.questions.forEach((q, index) => {
            let optionsHTML = '';
            
            if (q.type === 'MCQ') {
                q.options.forEach(opt => {
                    optionsHTML += `
                        <label class="radio-option">
                            <input type="radio" name="ans_${q.id}" value="${opt}">
                            ${opt}
                        </label>
                    `;
                });
            } else if (q.type === 'TrueFalse') {
                optionsHTML += `
                    <label class="radio-option"><input type="radio" name="ans_${q.id}" value="true"> True</label>
                    <label class="radio-option"><input type="radio" name="ans_${q.id}" value="false"> False</label>
                `;
            } else if (q.type === 'ShortAnswer') {
                optionsHTML += `
                    <input type="text" name="ans_${q.id}" placeholder="Type your answer here..." style="width:100%; margin-top:0.5rem">
                `;
            }

            qContainer.innerHTML += `
                <div class="q-block" data-qid="${q.id}">
                    <div class="q-header">
                        <h4>Q${index+1}. ${q.text}</h4>
                        <span style="color:var(--text-secondary); font-size:0.9rem">[${q.marks} Marks]</span>
                    </div>
                    <div class="q-options-container">
                        ${optionsHTML}
                    </div>
                </div>
            `;
        });

    } catch (err) {
        showToast('Error loading quiz details', 'error');
    }
}

function backToStudentDashboard() {
    document.getElementById('student-dashboard').classList.remove('hidden');
    document.getElementById('quiz-taking-view').classList.add('hidden');
    document.getElementById('quiz-results-view').classList.add('hidden');
}

document.getElementById('submit-quiz-btn').addEventListener('click', async () => {
    const studentName = document.getElementById('student-name').value;
    if(!studentName) return showToast('Please enter your name!', 'warning');

    const blocks = document.querySelectorAll('#attempt-questions .q-block');
    const answers = [];

    blocks.forEach(b => {
        const qId = b.dataset.qid;
        // Check for radio or text input
        const checkedRadio = b.querySelector(`input[name="ans_${qId}"]:checked`);
        const textInput = b.querySelector(`input[name="ans_${qId}"][type="text"]`);
        
        let ansValue = "";
        if (checkedRadio) ansValue = checkedRadio.value;
        else if (textInput) ansValue = textInput.value;

        answers.push({ questionId: qId, answer: ansValue });
    });

    const payload = {
        studentName: studentName,
        answers: answers
    };

    try {
        const res = await fetch(`${API_BASE}/attempts/${currentAttemptQuizId}`, {
            method: 'POST',
            body: JSON.stringify(payload)
        });
        
        if(res.ok) {
            const results = await res.json();
            
            // Show results
            document.getElementById('quiz-taking-view').classList.add('hidden');
            document.getElementById('quiz-results-view').classList.remove('hidden');
            
            document.getElementById('final-score').textContent = results.score;
            document.getElementById('total-score').textContent = results.total;
            
            const pct = (results.score / results.total) * 100;
            if (pct >= 80) document.getElementById('feedback-message').textContent = 'Brilliant! You mastered this topic.';
            else if (pct >= 50) document.getElementById('feedback-message').textContent = 'Good job, but there is room for improvement.';
            else document.getElementById('feedback-message').textContent = 'Keep practicing! You will get better.';

        } else {
            showToast('Error submitting quiz', 'error');
        }
    } catch (err) {
        showToast('Failed to reach grading server', 'error');
    }
});
