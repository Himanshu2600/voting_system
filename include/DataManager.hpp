#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include "User.hpp"
#include "Voter.hpp"
#include "Admin.hpp"
#include "Candidate.hpp"
#include "Election.hpp"
#include "Vote.hpp"
#include "Ledger.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class DataManager {
private:
    std::string dataDir;
    std::unordered_map<std::string, Voter> voters;     // username -> Voter
    std::unordered_map<std::string, Admin> admins;     // username -> Admin
    std::unordered_map<std::string, Election> elections; // electionId -> Election
    std::vector<Candidate> candidates;                  // all candidates
    Ledger ledger;

public:
    explicit DataManager(std::string dataDir = "./data");

    bool loadAll();
    bool saveAll();
    void seedInitialData();

    // Authentication & User Management
    User* authenticate(const std::string& username, const std::string& password);
    bool registerVoter(const std::string& username, const std::string& password,
                       const std::string& fullName, const std::string& ssnLast4, std::string& outMsg);
    bool registerAdmin(const std::string& username, const std::string& password,
                       const std::string& adminCode, std::string& outMsg);
    std::vector<Voter> getAllVoters() const;
    bool approveVoter(const std::string& username);

    // Election Management
    bool createElection(const std::string& title, const std::string& description,
                        const std::string& startDate, const std::string& endDate, std::string& outId);
    bool updateElectionStatus(const std::string& electionId, ElectionStatus status);
    std::vector<Election> getAllElections() const;
    Election* getElection(const std::string& electionId);

    // Candidate Management
    bool addCandidate(const std::string& electionId, const std::string& name,
                      const std::string& party, const std::string& manifesto);
    std::vector<Candidate> getCandidatesForElection(const std::string& electionId) const;

    // Voting & Audit
    bool castVote(const std::string& voterUsername, const std::string& electionId,
                  const std::string& candidateId, std::string& outReceipt, std::string& outMsg);
    const Ledger& getLedger() const { return ledger; }
    Ledger& getLedger() { return ledger; }
    bool verifyLedger(std::vector<std::string>& auditLog) const;
    const Block* verifyReceipt(const std::string& receiptCode) const;
};

#endif // DATA_MANAGER_HPP
