#include "ConsoleUI.hpp"
#include "CryptoUtils.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <cstdlib>

// ANSI Colors
namespace Colors {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string B_CYAN  = "\033[1;36m";
    const std::string B_GREEN = "\033[1;32m";
    const std::string B_RED   = "\033[1;31m";
    const std::string B_YELLOW = "\033[1;33m";
}

ConsoleUI::ConsoleUI(DataManager& dm) : dataManager(dm), currentUser(nullptr) {}

void ConsoleUI::clearScreen() const {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void ConsoleUI::pressEnterToContinue() const {
    std::cout << "\n" << Colors::YELLOW << "Press Enter to continue..." << Colors::RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

std::string ConsoleUI::promptInput(const std::string& prompt) const {
    std::cout << Colors::CYAN << prompt << ": " << Colors::RESET;
    std::string input;
    std::getline(std::cin >> std::ws, input);
    return input;
}

std::string ConsoleUI::promptPassword(const std::string& prompt) const {
    // Standard terminal input for cross-platform stability
    std::cout << Colors::CYAN << prompt << ": " << Colors::RESET;
    std::string password;
    std::cin >> password;
    return password;
}

void ConsoleUI::showHeader(const std::string& title) const {
    clearScreen();
    std::cout << Colors::B_CYAN << "========================================================================\n";
    std::cout << "               SECURE ONLINE VOTING SYSTEM (CRYPTOGRAPHIC LEDGER)       \n";
    std::cout << "========================================================================\n" << Colors::RESET;
    std::cout << Colors::BOLD << Colors::YELLOW << " >> " << title << Colors::RESET << "\n";
    std::cout << Colors::CYAN << "------------------------------------------------------------------------\n" << Colors::RESET;
}

void ConsoleUI::start() {
    dataManager.loadAll();
    while (true) {
        showMainMenu();
    }
}

void ConsoleUI::showMainMenu() {
    showHeader("MAIN PORTAL");
    std::cout << Colors::WHITE << "  [1] Login (Voter / Admin)\n";
    std::cout << "  [2] Register New Voter Account\n";
    std::cout << "  [3] Register Admin Account\n";
    std::cout << "  [4] Verify Vote Receipt Code\n";
    std::cout << "  [5] View Active Elections & Candidates\n";
    std::cout << "  [0] Exit Application\n" << Colors::RESET;
    std::cout << Colors::CYAN << "------------------------------------------------------------------------\n" << Colors::RESET;

    std::string choice = promptInput("Select an option");

    if (choice == "1") {
        handleLogin();
    } else if (choice == "2") {
        handleVoterRegistration();
    } else if (choice == "3") {
        handleAdminRegistration();
    } else if (choice == "4") {
        handleVerifyReceipt();
    } else if (choice == "5") {
        handleViewResults();
    } else if (choice == "0") {
        dataManager.saveAll();
        std::cout << "\n" << Colors::GREEN << "Thank you for using the Secure Online Voting System. Goodbye!\n" << Colors::RESET;
        std::exit(0);
    } else {
        std::cout << Colors::RED << "Invalid option selected.\n" << Colors::RESET;
        pressEnterToContinue();
    }
}

void ConsoleUI::handleLogin() {
    showHeader("ACCOUNT LOGIN");
    std::string username = promptInput("Enter Username");
    std::string password = promptPassword("Enter Password");

    User* user = dataManager.authenticate(username, password);

    if (user == nullptr) {
        std::cout << "\n" << Colors::B_RED << "[ERROR] Invalid username or password.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    currentUser = user;
    std::cout << "\n" << Colors::B_GREEN << "[SUCCESS] Authenticated successfully as "
              << user->getRoleString() << " (" << user->getUsername() << ")!" << Colors::RESET << "\n";
    pressEnterToContinue();

    if (user->getRole() == UserRole::ADMIN) {
        Admin* admin = dynamic_cast<Admin*>(user);
        while (currentUser != nullptr) {
            showAdminMenu(admin);
        }
    } else {
        Voter* voter = dynamic_cast<Voter*>(user);
        while (currentUser != nullptr) {
            showVoterMenu(voter);
        }
    }
}

void ConsoleUI::handleVoterRegistration() {
    showHeader("NEW VOTER REGISTRATION");
    std::string username = promptInput("Choose Username");
    std::string password = promptPassword("Choose Password");
    std::string fullName = promptInput("Full Name");
    std::string ssnLast4 = promptInput("National ID / SSN Last 4 digits");

    std::string msg;
    if (dataManager.registerVoter(username, password, fullName, ssnLast4, msg)) {
        std::cout << "\n" << Colors::B_GREEN << msg << Colors::RESET << "\n";
    } else {
        std::cout << "\n" << Colors::B_RED << msg << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleAdminRegistration() {
    showHeader("ADMIN ACCOUNT REGISTRATION");
    std::string username = promptInput("Choose Admin Username");
    std::string password = promptPassword("Choose Password");
    std::string adminCode = promptInput("Authorization Secret Code (e.g., ADMIN2026)");

    std::string msg;
    if (dataManager.registerAdmin(username, password, adminCode, msg)) {
        std::cout << "\n" << Colors::B_GREEN << msg << Colors::RESET << "\n";
    } else {
        std::cout << "\n" << Colors::B_RED << msg << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::showVoterMenu(Voter* voter) {
    showHeader("VOTER DASHBOARD - Welcome, " + voter->getFullName());
    std::cout << Colors::WHITE << "Account Status: "
              << (voter->getIsApproved() ? Colors::GREEN + "APPROVED" : Colors::RED + "PENDING APPROVAL")
              << Colors::WHITE << "\n\n";

    std::cout << "  [1] Cast Vote in Active Election\n";
    std::cout << "  [2] Verify My Vote Receipt Code\n";
    std::cout << "  [3] View Elections & Candidate Manifestos\n";
    std::cout << "  [4] View Live / Final Results\n";
    std::cout << "  [0] Logout\n" << Colors::RESET;
    std::cout << Colors::CYAN << "------------------------------------------------------------------------\n" << Colors::RESET;

    std::string choice = promptInput("Select an option");

    if (choice == "1") {
        handleCastVote(voter);
    } else if (choice == "2") {
        handleVerifyReceipt();
    } else if (choice == "3") {
        handleViewResults();
    } else if (choice == "4") {
        handleViewResults();
    } else if (choice == "0") {
        currentUser = nullptr;
        std::cout << Colors::YELLOW << "Logged out successfully.\n" << Colors::RESET;
        pressEnterToContinue();
    } else {
        std::cout << Colors::RED << "Invalid option.\n" << Colors::RESET;
        pressEnterToContinue();
    }
}

void ConsoleUI::handleCastVote(Voter* voter) {
    showHeader("CAST YOUR VOTE");

    auto elections = dataManager.getAllElections();
    std::vector<Election> activeElections;
    for (const auto& e : elections) {
        if (e.getStatus() == ElectionStatus::ACTIVE) {
            activeElections.push_back(e);
        }
    }

    if (activeElections.empty()) {
        std::cout << Colors::YELLOW << "No active elections available for voting at this time.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    std::cout << Colors::BOLD << std::setw(15) << "ELECTION ID" << std::setw(35) << "TITLE" << std::setw(15) << "STATUS" << Colors::RESET << "\n";
    std::cout << "------------------------------------------------------------------------\n";
    for (const auto& e : activeElections) {
        bool alreadyVoted = voter->hasVotedInElection(e.getId());
        std::string statusTag = alreadyVoted ? Colors::RED + "[VOTED]" + Colors::RESET : Colors::GREEN + "[READY]" + Colors::RESET;
        std::cout << std::setw(15) << e.getId() << std::setw(35) << e.getTitle() << std::setw(25) << statusTag << "\n";
    }

    std::string elecId = promptInput("\nEnter Election ID to vote in (or '0' to cancel)");
    if (elecId == "0") return;

    if (voter->hasVotedInElection(elecId)) {
        std::cout << "\n" << Colors::B_RED << "[ALERT] You have ALREADY cast your vote in this election! Double voting is prohibited." << Colors::RESET << "\n";
        pressEnterToContinue();
        return;
    }

    auto candidates = dataManager.getCandidatesForElection(elecId);
    if (candidates.empty()) {
        std::cout << Colors::RED << "No candidates registered for this election yet.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    std::cout << "\n" << Colors::B_CYAN << "--- CANDIDATE LIST ---" << Colors::RESET << "\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << Colors::BOLD << " [" << (i + 1) << "] " << candidates[i].getName()
                  << Colors::RESET << " (" << candidates[i].getParty() << ") [ID: " << candidates[i].getId() << "]\n";
        std::cout << "     Manifesto: " << candidates[i].getManifesto() << "\n\n";
    }

    std::string candChoice = promptInput("Enter Candidate ID (or option number 1-" + std::to_string(candidates.size()) + ")");
    std::string candId = candChoice;

    try {
        int idx = std::stoi(candChoice);
        if (idx >= 1 && idx <= static_cast<int>(candidates.size())) {
            candId = candidates[idx - 1].getId();
        }
    } catch (...) {}

    std::cout << "\n" << Colors::B_YELLOW << "CONFIRMATION: Are you sure you want to cast your vote for candidate ID: " << candId << "? (y/n): " << Colors::RESET;
    std::string confirm;
    std::cin >> confirm;

    if (confirm != "y" && confirm != "Y") {
        std::cout << Colors::RED << "Vote canceled by user.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    std::string receiptCode, msg;
    bool success = dataManager.castVote(voter->getUsername(), elecId, candId, receiptCode, msg);

    if (success) {
        std::cout << "\n" << Colors::B_GREEN << "========================================================================\n";
        std::cout << "                  VOTE RECORDED SUCCESSFULLY!                           \n";
        std::cout << "========================================================================\n" << Colors::RESET;
        std::cout << Colors::BOLD << "YOUR ANONYMOUS VERIFICATION RECEIPT CODE:\n\n";
        std::cout << "  " << Colors::B_CYAN << receiptCode << Colors::RESET << "\n\n";
        std::cout << Colors::YELLOW << "PLEASE SAVE THIS RECEIPT CODE!\n"
                  << "You can use this code anytime to audit the cryptographic ledger and verify\n"
                  << "that your vote has been securely included without revealing your selection.\n"
                  << Colors::RESET;
    } else {
        std::cout << "\n" << Colors::B_RED << msg << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleVerifyReceipt() {
    showHeader("VOTE RECEIPT AUDIT VERIFICATION");
    std::string receipt = promptInput("Enter Receipt Code (e.g., RCP-8F92A1...)");

    const Block* block = dataManager.verifyReceipt(receipt);
    if (block != nullptr) {
        std::cout << "\n" << Colors::B_GREEN << "[VERIFIED] RECEIPT VALID & INCLUDED IN LEDGER!" << Colors::RESET << "\n";
        std::cout << Colors::CYAN << "------------------------------------------------------------------------\n" << Colors::RESET;
        std::cout << "Block Index   : #" << block->index << "\n";
        std::cout << "Timestamp     : " << block->timestamp << "\n";
        std::cout << "Election ID   : " << block->vote.getElectionId() << "\n";
        std::cout << "Block Hash    : " << block->blockHash.substr(0, 32) << "...\n";
        std::cout << "Previous Hash : " << block->previousHash.substr(0, 32) << "...\n";
        std::cout << Colors::GREEN << "\nIntegrity Status: Cryptographically linked & verified in vote ledger.\n" << Colors::RESET;
    } else {
        std::cout << "\n" << Colors::B_RED << "[NOT FOUND] Receipt code was not found in the cryptographic vote ledger." << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleViewResults() {
    showHeader("ELECTIONS & LIVE RESULTS");
    auto elections = dataManager.getAllElections();

    if (elections.empty()) {
        std::cout << Colors::YELLOW << "No elections found in system.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    for (const auto& e : elections) {
        std::cout << Colors::B_CYAN << "------------------------------------------------------------------------\n";
        std::cout << "ELECTION: " << e.getTitle() << " [ID: " << e.getId() << "]\n";
        std::cout << "Status  : " << e.getStatusString() << " | Total Votes Cast: " << e.getTotalVotesCast() << "\n";
        std::cout << "Description: " << e.getDescription() << "\n";
        std::cout << "------------------------------------------------------------------------\n" << Colors::RESET;

        auto cands = dataManager.getCandidatesForElection(e.getId());
        if (cands.empty()) {
            std::cout << "  (No candidates registered yet)\n\n";
            continue;
        }

        int maxVotes = -1;
        std::string winnerName = "None";

        for (const auto& c : cands) {
            double pct = (e.getTotalVotesCast() == 0) ? 0.0 : (double)c.getVoteCount() / e.getTotalVotesCast() * 100.0;
            std::cout << "  * " << std::setw(22) << std::left << c.getName()
                      << " (" << std::setw(20) << c.getParty() << "): "
                      << std::setw(5) << c.getVoteCount() << " votes ["
                      << std::fixed << std::setprecision(1) << pct << "%]\n";

            if (c.getVoteCount() > maxVotes) {
                maxVotes = c.getVoteCount();
                winnerName = c.getName();
            }
        }

        if (e.getStatus() == ElectionStatus::CLOSED && e.getTotalVotesCast() > 0) {
            std::cout << "\n" << Colors::B_GREEN << "  >>> WINNER DECLARED: " << winnerName << " (" << maxVotes << " votes) <<<\n" << Colors::RESET;
        }
        std::cout << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::showAdminMenu(Admin* admin) {
    showHeader("ADMINISTRATION CONTROL PANEL - Logged in as: " + admin->getUsername());
    std::cout << Colors::WHITE << "  [1] Create New Election\n";
    std::cout << "  [2] Add Candidate to Election\n";
    std::cout << "  [3] Manage Election Status (Open / Close Election)\n";
    std::cout << "  [4] Audit Ledger Cryptographic Integrity\n";
    std::cout << "  [5] View Registered Voters List\n";
    std::cout << "  [6] Export Full Audit & Results Report to File\n";
    std::cout << "  [7] View Elections & Live Standings\n";
    std::cout << "  [0] Logout\n" << Colors::RESET;
    std::cout << Colors::CYAN << "------------------------------------------------------------------------\n" << Colors::RESET;

    std::string choice = promptInput("Select Admin Action");

    if (choice == "1") {
        handleCreateElection();
    } else if (choice == "2") {
        handleAddCandidate();
    } else if (choice == "3") {
        handleManageElectionStatus();
    } else if (choice == "4") {
        handleAuditLedger();
    } else if (choice == "5") {
        handleViewVoters();
    } else if (choice == "6") {
        handleExportReport();
    } else if (choice == "7") {
        handleViewResults();
    } else if (choice == "0") {
        currentUser = nullptr;
        std::cout << Colors::YELLOW << "Admin logged out.\n" << Colors::RESET;
        pressEnterToContinue();
    } else {
        std::cout << Colors::RED << "Invalid option.\n" << Colors::RESET;
        pressEnterToContinue();
    }
}

void ConsoleUI::handleCreateElection() {
    showHeader("CREATE NEW ELECTION");
    std::string title = promptInput("Election Title");
    std::string desc = promptInput("Description");
    std::string startDate = promptInput("Start Date (YYYY-MM-DD)");
    std::string endDate = promptInput("End Date (YYYY-MM-DD)");

    std::string outId;
    if (dataManager.createElection(title, desc, startDate, endDate, outId)) {
        std::cout << "\n" << Colors::B_GREEN << "[SUCCESS] Election created successfully! Assigned ID: " << outId << Colors::RESET << "\n";
    } else {
        std::cout << "\n" << Colors::B_RED << "[ERROR] Failed to create election." << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleAddCandidate() {
    showHeader("ADD CANDIDATE TO ELECTION");
    auto elections = dataManager.getAllElections();

    if (elections.empty()) {
        std::cout << Colors::RED << "No elections exist. Please create an election first.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    std::cout << Colors::CYAN << "Existing Elections:\n" << Colors::RESET;
    for (const auto& e : elections) {
        std::cout << "  - ID: " << std::setw(15) << std::left << e.getId() << " Title: " << e.getTitle() << "\n";
    }

    std::string elecId = promptInput("\nEnter Election ID");
    std::string name = promptInput("Candidate Full Name");
    std::string party = promptInput("Party / Affiliation");
    std::string manifesto = promptInput("Manifesto / Key Platform");

    if (dataManager.addCandidate(elecId, name, party, manifesto)) {
        std::cout << "\n" << Colors::B_GREEN << "[SUCCESS] Candidate " << name << " added to election " << elecId << Colors::RESET << "\n";
    } else {
        std::cout << "\n" << Colors::B_RED << "[ERROR] Invalid Election ID." << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleManageElectionStatus() {
    showHeader("MANAGE ELECTION STATUS");
    auto elections = dataManager.getAllElections();

    if (elections.empty()) {
        std::cout << Colors::RED << "No elections available.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    for (const auto& e : elections) {
        std::cout << "  ID: " << std::setw(15) << std::left << e.getId()
                  << " Status: " << std::setw(10) << e.getStatusString()
                  << " Title: " << e.getTitle() << "\n";
    }

    std::string elecId = promptInput("\nEnter Election ID to update");
    std::cout << "Select New Status: [1] ACTIVE (Open Voting)  [2] CLOSED (End Voting & Declare Winner)  [3] UPCOMING\n";
    std::string choice = promptInput("Choice");

    ElectionStatus st = ElectionStatus::UPCOMING;
    if (choice == "1") st = ElectionStatus::ACTIVE;
    else if (choice == "2") st = ElectionStatus::CLOSED;

    if (dataManager.updateElectionStatus(elecId, st)) {
        std::cout << "\n" << Colors::B_GREEN << "[SUCCESS] Election status updated." << Colors::RESET << "\n";
    } else {
        std::cout << "\n" << Colors::B_RED << "[ERROR] Election ID not found." << Colors::RESET << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleAuditLedger() {
    showHeader("CRYPTOGRAPHIC LEDGER INTEGRITY AUDIT");
    std::cout << Colors::YELLOW << "Running SHA-256 hash-chain verification across all vote blocks...\n\n" << Colors::RESET;

    std::vector<std::string> auditLog;
    bool isValid = dataManager.verifyLedger(auditLog);

    for (const auto& logEntry : auditLog) {
        if (logEntry.find("[SUCCESS]") != std::string::npos) {
            std::cout << Colors::B_GREEN << logEntry << Colors::RESET << "\n";
        } else if (logEntry.find("[CORRUPTED]") != std::string::npos) {
            std::cout << Colors::B_RED << logEntry << Colors::RESET << "\n";
        } else {
            std::cout << Colors::YELLOW << logEntry << Colors::RESET << "\n";
        }
    }

    std::cout << "\n" << Colors::CYAN << "Total Ledger Blocks: " << dataManager.getLedger().getSize() << Colors::RESET << "\n";
    pressEnterToContinue();
}

void ConsoleUI::handleViewVoters() {
    showHeader("REGISTERED VOTERS DATABASE");
    auto voters = dataManager.getAllVoters();

    if (voters.empty()) {
        std::cout << Colors::YELLOW << "No voters registered.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    std::cout << std::left << std::setw(12) << "VOTER ID"
              << std::setw(15) << "USERNAME"
              << std::setw(22) << "FULL NAME"
              << std::setw(12) << "STATUS"
              << std::setw(15) << "VOTES CAST" << "\n";
    std::cout << "------------------------------------------------------------------------\n";

    for (const auto& v : voters) {
        std::cout << std::left << std::setw(12) << v.getId()
                  << std::setw(15) << v.getUsername()
                  << std::setw(22) << v.getFullName()
                  << std::setw(12) << (v.getIsApproved() ? "Approved" : "Pending")
                  << std::setw(15) << v.getVotedElectionIds().size() << "\n";
    }
    pressEnterToContinue();
}

void ConsoleUI::handleExportReport() {
    showHeader("EXPORT ELECTION REPORT");
    std::string filename = "data/election_audit_report.txt";
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << Colors::RED << "Could not open file for writing.\n" << Colors::RESET;
        pressEnterToContinue();
        return;
    }

    file << "========================================================================\n";
    file << "               OFFICIAL ONLINE VOTING SYSTEM AUDIT REPORT               \n";
    file << "========================================================================\n";
    file << "Generated at: " << CryptoUtils::getCurrentTimestamp() << "\n\n";

    file << "--- ELECTION RESULTS ---\n";
    auto elections = dataManager.getAllElections();
    for (const auto& e : elections) {
        file << "\nElection: " << e.getTitle() << " [ID: " << e.getId() << "]\n";
        file << "Status  : " << e.getStatusString() << " | Total Votes: " << e.getTotalVotesCast() << "\n";
        file << "Candidates:\n";

        auto cands = dataManager.getCandidatesForElection(e.getId());
        for (const auto& c : cands) {
            double pct = (e.getTotalVotesCast() == 0) ? 0.0 : (double)c.getVoteCount() / e.getTotalVotesCast() * 100.0;
            file << "  - " << c.getName() << " (" << c.getParty() << "): "
                 << c.getVoteCount() << " votes (" << pct << "%)\n";
        }
    }

    file << "\n--- CRYPTOGRAPHIC LEDGER AUDIT ---\n";
    std::vector<std::string> auditLog;
    bool isValid = dataManager.verifyLedger(auditLog);
    for (const auto& line : auditLog) {
        file << line << "\n";
    }

    file.close();

    std::cout << Colors::B_GREEN << "[SUCCESS] Full election audit report exported to file:\n"
              << Colors::B_CYAN << "  " << filename << Colors::RESET << "\n";
    pressEnterToContinue();
}
