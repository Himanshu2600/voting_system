#include "DataManager.hpp"
#include "CryptoUtils.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

DataManager::DataManager(std::string dataDir) : dataDir(dataDir) {}

bool DataManager::loadAll() {
    // 1. Load Voters
    std::ifstream voterFile(dataDir + "/voters.dat");
    if (voterFile.is_open()) {
        std::string line;
        while (std::getline(voterFile, line)) {
            if (!line.empty()) {
                Voter v = Voter::deserializeVoter(line);
                voters[v.getUsername()] = v;
            }
        }
        voterFile.close();
    }

    // 2. Load Admins
    std::ifstream adminFile(dataDir + "/admins.dat");
    if (adminFile.is_open()) {
        std::string line;
        while (std::getline(adminFile, line)) {
            if (!line.empty()) {
                Admin a = Admin::deserializeAdmin(line);
                admins[a.getUsername()] = a;
            }
        }
        adminFile.close();
    }

    // 3. Load Elections
    std::ifstream elecFile(dataDir + "/elections.dat");
    if (elecFile.is_open()) {
        std::string line;
        while (std::getline(elecFile, line)) {
            if (!line.empty()) {
                Election e = Election::deserialize(line);
                elections[e.getId()] = e;
            }
        }
        elecFile.close();
    }

    // 4. Load Candidates
    std::ifstream candFile(dataDir + "/candidates.dat");
    if (candFile.is_open()) {
        std::string line;
        while (std::getline(candFile, line)) {
            if (!line.empty()) {
                candidates.push_back(Candidate::deserialize(line));
            }
        }
        candFile.close();
    }

    // 5. Load Ledger
    std::ifstream ledgerFile(dataDir + "/ledger.dat");
    if (ledgerFile.is_open()) {
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(ledgerFile, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        ledgerFile.close();
        ledger.loadFromSerialized(lines);
    }

    if (voters.empty() && admins.empty() && elections.empty()) {
        seedInitialData();
    }
    return true;
}

bool DataManager::saveAll() {
    // 1. Save Voters
    std::ofstream voterFile(dataDir + "/voters.dat");
    if (voterFile.is_open()) {
        for (const auto& pair : voters) {
            voterFile << pair.second.serialize() << "\n";
        }
        voterFile.close();
    }

    // 2. Save Admins
    std::ofstream adminFile(dataDir + "/admins.dat");
    if (adminFile.is_open()) {
        for (const auto& pair : admins) {
            adminFile << pair.second.serialize() << "\n";
        }
        adminFile.close();
    }

    // 3. Save Elections
    std::ofstream elecFile(dataDir + "/elections.dat");
    if (elecFile.is_open()) {
        for (const auto& pair : elections) {
            elecFile << pair.second.serialize() << "\n";
        }
        elecFile.close();
    }

    // 4. Save Candidates
    std::ofstream candFile(dataDir + "/candidates.dat");
    if (candFile.is_open()) {
        for (const auto& cand : candidates) {
            candFile << cand.serialize() << "\n";
        }
        candFile.close();
    }

    // 5. Save Ledger
    std::ofstream ledgerFile(dataDir + "/ledger.dat");
    if (ledgerFile.is_open()) {
        ledgerFile << ledger.serialize();
        ledgerFile.close();
    }

    return true;
}

void DataManager::seedInitialData() {
    // Create default Admin: username 'admin', password 'admin123'
    std::string salt = CryptoUtils::generateSalt(16);
    std::string passHash = CryptoUtils::hashPassword("admin123", salt);
    Admin defaultAdmin("ADM-001", "admin", passHash, salt, "MASTER_SECRET", CryptoUtils::getCurrentTimestamp());
    admins["admin"] = defaultAdmin;

    // Create default Voter 1: username 'voter1', password 'voter123'
    std::string vSalt1 = CryptoUtils::generateSalt(16);
    std::string vHash1 = CryptoUtils::hashPassword("voter123", vSalt1);
    Voter voter1("VOT-101", "voter1", vHash1, vSalt1, "Alice Smith", CryptoUtils::sha256("SSN-1234"), true, CryptoUtils::getCurrentTimestamp());
    voters["voter1"] = voter1;

    // Create default Voter 2: username 'voter2', password 'voter123'
    std::string vSalt2 = CryptoUtils::generateSalt(16);
    std::string vHash2 = CryptoUtils::hashPassword("voter123", vSalt2);
    Voter voter2("VOT-102", "voter2", vHash2, vSalt2, "Bob Jones", CryptoUtils::sha256("SSN-5678"), true, CryptoUtils::getCurrentTimestamp());
    voters["voter2"] = voter2;

    // Create Sample Election 1
    std::string elecId1 = "ELEC-2026-PRES";
    Election e1(elecId1, "2026 National Presidential Election", "Official election for Head of Executive Council.",
                ElectionStatus::ACTIVE, "2026-07-01", "2026-08-01", 0);
    elections[elecId1] = e1;

    // Candidates for Presidential Election
    candidates.push_back(Candidate("CND-101", elecId1, "Elena Rostova", "Progressive Alliance", "Focusing on renewable energy, innovation, and educational tech."));
    candidates.push_back(Candidate("CND-102", elecId1, "Marcus Vance", "Liberty Coalition", "Focusing on tax reform, small business growth, and infrastructure."));
    candidates.push_back(Candidate("CND-103", elecId1, "Dr. Sarah Chen", "Independent Science Front", "Focusing on AI governance, healthcare access, and environmental policy."));

    // Create Sample Election 2
    std::string elecId2 = "ELEC-2026-STU";
    Election e2(elecId2, "University Student Council President", "Election for Student Body Representative.",
                ElectionStatus::ACTIVE, "2026-07-15", "2026-07-30", 0);
    elections[elecId2] = e2;

    candidates.push_back(Candidate("CND-201", elecId2, "David Kim", "Campus Unity", "Better campus cafeteria options, 24/7 library access."));
    candidates.push_back(Candidate("CND-202", elecId2, "Priya Sharma", "Student Rights Voice", "Lower tuition fees, more mental health support."));

    // Genesis Ledger block
    ledger.createGenesisBlock();

    saveAll();
}

User* DataManager::authenticate(const std::string& username, const std::string& password) {
    auto adminIt = admins.find(username);
    if (adminIt != admins.end()) {
        if (adminIt->second.verifyPassword(password)) {
            return &(adminIt->second);
        }
    }

    auto voterIt = voters.find(username);
    if (voterIt != voters.end()) {
        if (voterIt->second.verifyPassword(password)) {
            return &(voterIt->second);
        }
    }

    return nullptr;
}

bool DataManager::registerVoter(const std::string& username, const std::string& password,
                                const std::string& fullName, const std::string& ssnLast4, std::string& outMsg) {
    if (voters.count(username) > 0 || admins.count(username) > 0) {
        outMsg = "Username already exists. Please choose a different username.";
        return false;
    }

    std::string id = "VOT-" + std::to_string(voters.size() + 1001);
    std::string salt = CryptoUtils::generateSalt(16);
    std::string passHash = CryptoUtils::hashPassword(password, salt);
    std::string natIdHash = CryptoUtils::sha256(ssnLast4);
    std::string ts = CryptoUtils::getCurrentTimestamp();

    // Default approved for demo simplicity
    Voter newVoter(id, username, passHash, salt, fullName, natIdHash, true, ts);
    voters[username] = newVoter;

    saveAll();
    outMsg = "Registration successful! Account ID: " + id;
    return true;
}

bool DataManager::registerAdmin(const std::string& username, const std::string& password,
                                const std::string& adminCode, std::string& outMsg) {
    if (voters.count(username) > 0 || admins.count(username) > 0) {
        outMsg = "Username already exists.";
        return false;
    }

    if (adminCode != "ADMIN2026" && adminCode != "MASTER_SECRET") {
        outMsg = "Invalid Admin Registration Authorization Code.";
        return false;
    }

    std::string id = "ADM-" + std::to_string(admins.size() + 101);
    std::string salt = CryptoUtils::generateSalt(16);
    std::string passHash = CryptoUtils::hashPassword(password, salt);
    std::string ts = CryptoUtils::getCurrentTimestamp();

    Admin newAdmin(id, username, passHash, salt, adminCode, ts);
    admins[username] = newAdmin;

    saveAll();
    outMsg = "Admin account registered successfully! ID: " + id;
    return true;
}

std::vector<Voter> DataManager::getAllVoters() const {
    std::vector<Voter> result;
    for (const auto& pair : voters) {
        result.push_back(pair.second);
    }
    return result;
}

bool DataManager::approveVoter(const std::string& username) {
    auto it = voters.find(username);
    if (it != voters.end()) {
        it->second.setApproved(true);
        saveAll();
        return true;
    }
    return false;
}

bool DataManager::createElection(const std::string& title, const std::string& description,
                                 const std::string& startDate, const std::string& endDate, std::string& outId) {
    outId = "ELEC-" + std::to_string(elections.size() + 101);
    Election e(outId, title, description, ElectionStatus::ACTIVE, startDate, endDate, 0);
    elections[outId] = e;
    saveAll();
    return true;
}

bool DataManager::updateElectionStatus(const std::string& electionId, ElectionStatus status) {
    auto it = elections.find(electionId);
    if (it != elections.end()) {
        it->second.setStatus(status);
        saveAll();
        return true;
    }
    return false;
}

std::vector<Election> DataManager::getAllElections() const {
    std::vector<Election> result;
    for (const auto& pair : elections) {
        result.push_back(pair.second);
    }
    return result;
}

Election* DataManager::getElection(const std::string& electionId) {
    auto it = elections.find(electionId);
    if (it != elections.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool DataManager::addCandidate(const std::string& electionId, const std::string& name,
                               const std::string& party, const std::string& manifesto) {
    if (elections.count(electionId) == 0) return false;
    std::string cid = "CND-" + std::to_string(candidates.size() + 101);
    candidates.push_back(Candidate(cid, electionId, name, party, manifesto, 0));
    saveAll();
    return true;
}

std::vector<Candidate> DataManager::getCandidatesForElection(const std::string& electionId) const {
    std::vector<Candidate> result;
    for (const auto& cand : candidates) {
        if (cand.getElectionId() == electionId) {
            result.push_back(cand);
        }
    }
    return result;
}

bool DataManager::castVote(const std::string& voterUsername, const std::string& electionId,
                           const std::string& candidateId, std::string& outReceipt, std::string& outMsg) {
    auto voterIt = voters.find(voterUsername);
    if (voterIt == voters.end()) {
        outMsg = "Voter account not found.";
        return false;
    }

    Voter& voter = voterIt->second;
    if (!voter.getIsApproved()) {
        outMsg = "Your voter account is not yet approved by an admin.";
        return false;
    }

    if (voter.hasVotedInElection(electionId)) {
        outMsg = "SECURITY ERROR: You have already cast a vote in this election. Double voting is prohibited!";
        return false;
    }

    auto elecIt = elections.find(electionId);
    if (elecIt == elections.end()) {
        outMsg = "Election ID invalid.";
        return false;
    }

    Election& election = elecIt->second;
    if (election.getStatus() != ElectionStatus::ACTIVE) {
        outMsg = "This election is currently not active (Status: " + election.getStatusString() + ").";
        return false;
    }

    // Find candidate and increment vote
    bool candidateFound = false;
    for (auto& cand : candidates) {
        if (cand.getElectionId() == electionId && cand.getId() == candidateId) {
            cand.incrementVotes();
            candidateFound = true;
            break;
        }
    }

    if (!candidateFound) {
        outMsg = "Candidate ID invalid for this election.";
        return false;
    }

    std::string timestamp = CryptoUtils::getCurrentTimestamp();
    outReceipt = CryptoUtils::generateReceiptCode(voter.getId(), electionId, timestamp);
    std::string voteId = "VT-" + std::to_string(ledger.getSize() + 1000);

    Vote vote(voteId, electionId, candidateId, outReceipt, timestamp);

    // Append to tamper-evident ledger
    ledger.addVote(vote);

    // Record vote in voter account & election
    voter.recordVote(electionId);
    election.incrementTotalVotes();

    saveAll();

    outMsg = "SUCCESS: Your vote has been recorded securely in the immutable cryptographic ledger.";
    return true;
}

bool DataManager::verifyLedger(std::vector<std::string>& auditLog) const {
    return ledger.verifyIntegrity(auditLog);
}

const Block* DataManager::verifyReceipt(const std::string& receiptCode) const {
    return ledger.findVoteByReceipt(receiptCode);
}
