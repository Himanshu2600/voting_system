cmake_minimum_required(VERSION 3.12)
project(OnlineVotingSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CORE_SOURCES
    src/CryptoUtils.cpp
    src/User.cpp
    src/Voter.cpp
    src/Admin.cpp
    src/Candidate.cpp
    src/Vote.cpp
    src/Election.cpp
    src/Ledger.cpp
    src/DataManager.cpp
)

# Target 1: CLI Application
add_executable(voting_system ${CORE_SOURCES} src/ConsoleUI.cpp src/main.cpp)

# Target 2: C++ Web Application Server
add_executable(cpp_voting_website_server ${CORE_SOURCES} src/HttpServer.cpp src/main_web.cpp)

# Target 3: Automated Test Runner
add_executable(test_runner ${CORE_SOURCES} src/test_system.cpp)
