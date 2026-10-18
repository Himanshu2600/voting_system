#!/bin/bash
echo "Building C++ Online Voting System & C++ Web Server Application..."

# Compile CLI Application
g++ -std=c++17 -Wall -Wextra -Iinclude src/CryptoUtils.cpp src/User.cpp src/Voter.cpp src/Admin.cpp src/Candidate.cpp src/Vote.cpp src/Election.cpp src/Ledger.cpp src/DataManager.cpp src/ConsoleUI.cpp src/main.cpp -o voting_system

# Compile C++ Web Application Server
g++ -std=c++17 -Wall -Wextra -Iinclude src/CryptoUtils.cpp src/User.cpp src/Voter.cpp src/Admin.cpp src/Candidate.cpp src/Vote.cpp src/Election.cpp src/Ledger.cpp src/DataManager.cpp src/HttpServer.cpp src/main_web.cpp -lpthread -o cpp_voting_website_server

# Compile Automated Test Suite
g++ -std=c++17 -Wall -Wextra -Iinclude src/CryptoUtils.cpp src/User.cpp src/Voter.cpp src/Admin.cpp src/Candidate.cpp src/Vote.cpp src/Election.cpp src/Ledger.cpp src/DataManager.cpp src/test_system.cpp -o test_runner

if [ $? -eq 0 ]; then
    echo "[SUCCESS] Build completed successfully!"
    echo "  - Run './voting_system' for Interactive Terminal Application"
    echo "  - Run './cpp_voting_website_server' for Native C++ Web Application Server (http://localhost:8080)"
else
    echo "[ERROR] Compilation failed."
fi
