#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include "DataManager.hpp"
#include <string>

class ConsoleUI {
private:
    DataManager& dataManager;
    User* currentUser;

public:
    explicit ConsoleUI(DataManager& dm);

    void start();

private:
    // Screen renders
    void showHeader(const std::string& title) const;
    void showMainMenu();
    void showVoterMenu(Voter* voter);
    void showAdminMenu(Admin* admin);

    // Voter actions
    void handleCastVote(Voter* voter);
    void handleVerifyReceipt();
    void handleViewResults();

    // Admin actions
    void handleCreateElection();
    void handleAddCandidate();
    void handleManageElectionStatus();
    void handleViewVoters();
    void handleAuditLedger();
    void handleExportReport();

    // Auth helpers
    void handleLogin();
    void handleVoterRegistration();
    void handleAdminRegistration();

    // Utility helpers
    std::string promptInput(const std::string& prompt) const;
    std::string promptPassword(const std::string& prompt) const;
    void pressEnterToContinue() const;
    void clearScreen() const;
};

#endif // CONSOLE_UI_HPP
