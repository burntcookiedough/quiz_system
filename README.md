# Quiz System - Full Documentation

## Table of Contents

1. [What Is This Project?](#1-what-is-this-project)
2. [How Does It Work?](#2-how-does-it-work)
3. [Project Folder Structure](#3-project-folder-structure)
4. [What You Need to Install First](#4-what-you-need-to-install-first)
5. [How to Build and Run the Project](#5-how-to-build-and-run-the-project)
6. [How to Use the Application Once It Is Running](#6-how-to-use-the-application-once-it-is-running)
7. [How to Package the Final Product](#7-how-to-package-the-final-product)
8. [Troubleshooting and Debugging](#8-troubleshooting-and-debugging)
9. [Frequently Asked Questions](#9-frequently-asked-questions)

---

## 1. What Is This Project?

This is a **Quiz Management System** built entirely in C++. It is a web-based application where:

- **Teachers** can create quizzes with multiple question types (Multiple Choice, True/False, Short Answer), view a dashboard of class performance, manage a student roster, and see reports.
- **Students** can browse available quizzes, take them in a timed environment, get auto-graded results instantly, and view their own performance history.

The project has two main parts:

1. **The Backend Server** (written in C++): This is the brain of the application. It handles all the data, stores quizzes as JSON files, grades student answers, and serves the web pages. It runs as a local HTTP server on your computer at port 8080.
2. **The Frontend** (written in HTML, CSS, and JavaScript): This is the visual part you interact with in your web browser. It includes the teacher dashboard, student hub, quiz builder, and quiz-taking pages.

When you run the project, the C++ backend starts a web server on your machine. You then open your web browser and go to `http://localhost:8080` to use the application. Everything runs locally on your own computer. No internet connection is needed after the initial setup.

---

## 2. How Does It Work?

Here is a simplified view of what happens when you run the project:

```
You double-click run.bat (Windows) or run ./run.sh (Mac)
        |
        v
The script checks if CMake and a C++ compiler are installed
        |
        v
CMake configures the project (reads CMakeLists.txt to understand what to build)
        |
        v
The compiler turns main.cpp into an executable file (QuizSystem.exe on Windows, QuizSystem on Mac)
        |
        v
The executable starts an HTTP server on http://localhost:8080
        |
        v
You open your browser, go to http://localhost:8080, and use the quiz system
```

The backend exposes a REST API. The frontend (HTML/JS pages in the `ui/` folder) makes HTTP requests to this API to fetch quizzes, submit answers, and display dashboards. The quiz data is stored as `.json` files inside the `data/` folder.

---

## 3. Project Folder Structure

Here is every folder and file in this project, and what each one does:

```
quiz_system/
|
|-- CMakeLists.txt          The build configuration file. CMake reads this to know
|                           what source files to compile and what libraries to link.
|
|-- run.bat                 The one-click build-and-run script for Windows.
|-- run.sh                  The one-click build-and-run script for macOS and Linux.
|-- package.bat             A script to bundle the final product into a .zip file (Windows).
|
|-- src/
|   |-- main.cpp            The main source code file. Contains all the server logic,
|                           API endpoints, quiz grading, and data setup.
|
|-- include/
|   |-- httplib.h           A third-party library for running a lightweight HTTP server.
|   |-- json.hpp            A third-party library for reading and writing JSON data.
|   |-- models/
|   |   |-- User.h          Base class for all users (teacher or student).
|   |   |-- Teacher.h       Inherits from User. Represents a teacher.
|   |   |-- Student.h       Inherits from User. Represents a student.
|   |   |-- Question.h      Abstract base class for all question types.
|   |   |-- MCQQuestion.h   Multiple Choice Question. Inherits from Question.
|   |   |-- TrueFalseQuestion.h   True/False Question. Inherits from Question.
|   |   |-- ShortAnswerQuestion.h Short Answer Question. Inherits from Question.
|   |   |-- Quiz.h          Represents a quiz containing a collection of questions.
|   |   |-- QuizAttempt.h   Represents a student's attempt at a quiz.
|   |-- interfaces/
|   |   |-- (interface files for abstraction)
|   |-- services/
|       |-- FileManager.h   Handles saving and loading quiz data to/from JSON files.
|
|-- ui/
|   |-- index.html              The landing page of the application.
|   |-- teacher-dashboard.html  The teacher's main dashboard with class metrics.
|   |-- teacher-roster.html     The teacher's student roster management page.
|   |-- quiz-builder.html       The page where teachers create new quizzes.
|   |-- student-hub.html        The student's main page showing available quizzes.
|   |-- quiz-take.html          The page where students take a quiz.
|   |-- student-reports.html    The student's performance report page.
|   |-- style.css               The main stylesheet for all pages.
|   |-- script.js               The main JavaScript file with shared frontend logic.
|   |-- css/                    Additional CSS files.
|   |-- js/                     Additional JavaScript files.
|
|-- data/
|   |-- Q001.json               A sample quiz stored as a JSON file.
|   |-- (other quiz files)      More quizzes are saved here as they are created.
|
|-- build/                      This folder is created automatically when you build the
|                               project. It contains the compiled executable and temporary
|                               build files. You should never edit anything inside this folder.
|
|-- .gitignore                  Tells Git which files to ignore (build artifacts, executables).
```

---

## 4. What You Need to Install First

You need exactly two things installed on your computer before the project can be built:

1. **CMake** - A tool that reads the `CMakeLists.txt` file and generates the instructions needed to compile the project.
2. **A C++ Compiler** - A program that translates the C++ source code into a binary executable your computer can actually run.

### Installation on Windows

**Step 1: Install CMake**

1. Go to https://cmake.org/download/
2. Under "Latest Release", find the file that says `Windows x64 Installer` (it will be a `.msi` file). Click on it to download.
3. Open the downloaded `.msi` file. The installer will launch.
4. Click "Next" through the screens.
5. **IMPORTANT**: When you reach the screen that says "Install Options", you will see a choice about PATH. Select the option that says **"Add CMake to the system PATH for all users"**. If you skip this step, the scripts will not be able to find CMake and will fail.
6. Click "Next" and then "Install". Wait for it to finish, then click "Finish".

**Step 2: Install a C++ Compiler**

You have two options. Pick whichever one you prefer.

Option A: Install Visual Studio (recommended for beginners)
1. Go to https://visualstudio.microsoft.com/downloads/
2. Download the **Community** edition (it is completely free).
3. Open the installer. It will show you a list of "Workloads".
4. Check the box next to **"Desktop development with C++"**.
5. Click "Install". This will download several gigabytes, so it may take a while.

Option B: Install MinGW (lighter alternative)
1. Go to https://www.mingw-w64.org/ or https://winlibs.com/ and download a MinGW-w64 build.
2. Extract the downloaded archive to a folder like `C:\mingw64`.
3. Add the `bin` folder inside it (for example `C:\mingw64\bin`) to your system PATH environment variable.

**How to verify everything is installed correctly on Windows:**

Open the Start menu, type `cmd`, and press Enter to open a Command Prompt. Then type the following commands one at a time:

```
cmake --version
```

You should see output like `cmake version 3.xx.x`. If it says "not recognized", CMake is not installed correctly or not in your PATH.

```
g++ --version
```
or
```
cl
```

One of these should print a version number. If both say "not recognized", you do not have a C++ compiler installed.

### Installation on macOS

**Step 1: Install the Apple Command Line Developer Tools**

1. Open the Terminal application. You can find it by pressing Command + Space, typing "Terminal", and pressing Enter.
2. Type the following command and press Enter:

```
xcode-select --install
```

3. A dialog box will pop up asking if you want to install the tools. Click "Install" and wait for it to finish.

This gives you the `clang++` compiler, which is Apple's C++ compiler. It is free and comes directly from Apple.

**Step 2: Install CMake using Homebrew**

Homebrew is a package manager for Mac. If you do not have it, install it first:

1. Open the Terminal and paste this command:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Follow the on-screen instructions. Once Homebrew is installed, type:

```
brew install cmake
```

**How to verify everything is installed correctly on macOS:**

Open Terminal and type:

```
cmake --version
```

You should see a version number. Then type:

```
clang++ --version
```

You should see version information about the Apple clang compiler.

---

## 5. How to Build and Run the Project

### On Windows

1. Open the project folder in File Explorer.
2. Find the file named `run.bat`. It may appear as just `run` if file extensions are hidden.
3. Double-click it.

A black terminal window will appear. The script will automatically:
- Check that CMake and a compiler are installed.
- Delete any old build files to start fresh.
- Configure the project using CMake.
- Compile the source code into `QuizSystem.exe`.
- Start the server.

If it succeeds, you will see the message:

```
Build Successful! Starting System...
Starting Quiz API Server on http://localhost:8080
```

If it fails at any step, it will print an error message starting with `[ERROR]` and pause so you can read what went wrong.

**To stop the server:** Press `Ctrl + C` in the terminal window, or simply close the terminal window.

### On macOS or Linux

1. Open the Terminal application.
2. Navigate to the project folder. The easiest way is to type `cd ` (with a space after it), then drag and drop the project folder from Finder into the Terminal window, and press Enter.
3. If this is the first time running the script, make it executable by typing:

```
chmod +x run.sh
```

4. Run the script:

```
./run.sh
```

The script will do the same steps as the Windows version: check dependencies, clean old builds, configure, compile, and start the server.

**To stop the server:** Press `Ctrl + C` in the Terminal.

---

## 6. How to Use the Application Once It Is Running

Once the server is running and you see the message `Starting Quiz API Server on http://localhost:8080`, open your web browser (Chrome, Firefox, Edge, Safari, etc.) and go to:

```
http://localhost:8080
```

This will load the landing page. From there you can navigate to:

- **Teacher Dashboard** (`/teacher-dashboard.html`) - View class performance metrics, recent submissions, and activity trends.
- **Quiz Builder** (`/quiz-builder.html`) - Create new quizzes with multiple choice, true/false, and short answer questions.
- **Teacher Roster** (`/teacher-roster.html`) - View and manage the list of students.
- **Student Hub** (`/student-hub.html`) - Browse and take available quizzes.
- **Student Reports** (`/student-reports.html`) - View your own quiz results and performance history.

---

## 7. How to Package the Final Product

If you want to share the finished application with someone who does not have CMake or a compiler installed, you can package it into a standalone `.zip` file.

**Note:** This only works on Windows because the `.exe` file produced is a Windows binary. Executables are platform-specific. A Windows `.exe` will not run on a Mac, and a Mac binary will not run on Windows.

### Steps (Windows Only)

1. First, make sure the project has been built successfully by running `run.bat` at least once.
2. Close the running server (press Ctrl+C or close the terminal).
3. Double-click `package.bat`.
4. This will create a file called `QuizSystem_Release.zip` in the project folder.

This zip file contains:
- `QuizSystem.exe` - the compiled server.
- `ui/` - all the frontend HTML/CSS/JS files.
- `data/` - the quiz data files.

You can send this zip to anyone with a Windows computer. They extract it, double-click `QuizSystem.exe`, open `http://localhost:8080` in their browser, and it works. They do not need to install anything.

---

## 8. Troubleshooting and Debugging

If something goes wrong, follow the steps below in order. Each step checks one thing. Stop as soon as you find the problem.

### On Windows

Open the Start menu, type `cmd`, press Enter to open a Command Prompt. Then run these commands one by one.

**Step 1: Is CMake installed?**

```
cmake --version
```

Expected output: Something like `cmake version 3.28.1`.

If you see `'cmake' is not recognized as an internal or external command`: CMake is either not installed, or it was not added to your system PATH during installation. Reinstall CMake and make sure to select "Add CMake to system PATH for all users" during the installer.

**Step 2: Is a C++ compiler installed?**

```
where g++
```

or

```
where cl
```

Expected output: A file path like `C:\ProgramData\mingw64\mingw64\bin\g++.exe`.

If both commands say `INFO: Could not find files for the given pattern(s).`: You do not have a C++ compiler. Install Visual Studio with the "Desktop development with C++" workload, or install MinGW.

**Step 3: Can CMake configure the project?**

Navigate to the project folder first. Replace `PATH_TO_FOLDER` with the actual folder path:

```
cd PATH_TO_FOLDER
cmake -S . -B build -G "MinGW Makefiles"
```

(If you are using Visual Studio instead of MinGW, just use `cmake -S . -B build` without the `-G` flag.)

Expected output: The last line should say `-- Configuring done` and `-- Build files have been written to:`.

If it says `CMake Error`: Read the error message carefully. Common issues:
- `CMAKE_C_COMPILER not set`: Your compiler is not in the PATH. Verify Step 2.
- `Could not find CMakeLists.txt`: You are not in the correct folder. Make sure you navigated to the project root.

**Step 4: Does the code compile?**

```
cmake --build build --config Release
```

Expected output: `[100%] Built target QuizSystem`.

If you see lines with the word `error:` in them, there is a problem in the source code. The error message will tell you the exact file name and line number where the problem is.

**Step 5: Does the server start?**

```
.\build\QuizSystem.exe
```

Expected output: `Starting Quiz API Server on http://localhost:8080`.

If the server starts but immediately closes, another program may already be using port 8080. You can check this by running:

```
netstat -ano | findstr :8080
```

If this shows a result, another program is using port 8080. Close that program and try again.

**Step 6: Can you reach the server in your browser?**

Open your browser and go to `http://localhost:8080`. If the page does not load:
- Make sure the server is still running in the terminal (you should see the "Starting Quiz API Server" message and the cursor should be blinking).
- Make sure you typed the address correctly. It must be `http://` not `https://`.
- Try `http://127.0.0.1:8080` instead.

---

### On macOS

Open the Terminal application and run these commands one by one.

**Step 1: Is CMake installed?**

```
cmake --version
```

If you see `command not found`: Install CMake with `brew install cmake`. If Homebrew is also not installed, see Section 4 above.

**Step 2: Is the C++ compiler installed?**

```
clang++ --version
```

If you see `command not found`: Run `xcode-select --install` to install the Apple developer tools.

**Step 3: Can CMake configure the project?**

Navigate to the project folder. Replace `PATH_TO_FOLDER` with the actual folder path (or drag the folder into Terminal):

```
cd PATH_TO_FOLDER
cmake -S . -B build
```

Expected output: `-- Configuring done` and `-- Build files have been written to:`.

**Step 4: Does the code compile?**

```
cmake --build build --config Release
```

Expected output: `[100%] Built target QuizSystem`.

If you see `error:` lines, the source code has a problem at the file and line number shown.

**Step 5: Does the server start?**

```
./build/QuizSystem
```

Expected output: `Starting Quiz API Server on http://localhost:8080`.

If it says `Address already in use`, another program is using port 8080. Find and stop it:

```
lsof -i :8080
kill -9 <PID>
```

Replace `<PID>` with the number shown in the output of the first command.

**Step 6: Can you reach the server in your browser?**

Open Safari or Chrome and go to `http://localhost:8080`.

---

## 9. Frequently Asked Questions

**Q: Do I need an internet connection to use this?**
A: Only for the initial setup (downloading CMake and a compiler). Once those are installed and the project is built, everything runs offline on your own computer.

**Q: Can I send the compiled program to a friend?**
A: On Windows, yes. Run `package.bat` and send the resulting `QuizSystem_Release.zip`. Your friend does not need to install anything. On Mac, your friend will need to build it themselves using `run.sh` because Mac executables cannot be easily redistributed without code signing.

**Q: Where is the quiz data stored?**
A: Inside the `data/` folder as `.json` files. Each quiz gets its own file. If you delete these files, the quizzes are gone.

**Q: Can multiple students use it at the same time?**
A: Yes, as long as they are all connected to the same network and access the server using your computer's local IP address instead of `localhost`. For example, `http://192.168.1.x:8080` where `x` is your machine's IP on the network.

**Q: How do I reset everything?**
A: Delete the `build/` folder and the files inside the `data/` folder (except the sample ones you want to keep). Then double-click `run.bat` or run `./run.sh` again.
