@echo off
echo Building C++ Online Voting System for Windows...
IF EXIST "C:\Dev-Cpp\bin\g++.exe" (
    "C:\Dev-Cpp\bin\g++.exe" -std=c++17 -Iinclude src\CryptoUtils.cpp src\User.cpp src\Voter.cpp src\Admin.cpp src\Candidate.cpp src\Vote.cpp src\Election.cpp src\Ledger.cpp src\DataManager.cpp src\ConsoleUI.cpp src\main.cpp -o voting_system.exe
) ELSE (
    g++ -std=c++17 -Iinclude src\CryptoUtils.cpp src\User.cpp src\Voter.cpp src\Admin.cpp src\Candidate.cpp src\Vote.cpp src\Election.cpp src\Ledger.cpp src\DataManager.cpp src\ConsoleUI.cpp src\main.cpp -o voting_system.exe
)

IF %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build completed successfully! Run 'voting_system.exe' to start.
) ELSE (
    echo [ERROR] Compilation failed. Please ensure a C++17 compiler (g++, clang++, or MSVC) is installed and available in your PATH.
)
