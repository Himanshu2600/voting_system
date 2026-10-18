#include "Voter.hpp"
#include <sstream>

Voter::Voter() : User(), isApproved(false) {}

Voter::Voter(std::string id, std::string username, std::string passwordHash, std::string salt,
             std::string fullName, std::string nationalIdHash, bool isApproved, std::string createdAt)
    : User(id, username, passwordHash, salt, UserRole::VOTER, createdAt),
      fullName(fullName), nationalIdHash(nationalIdHash), isApproved(isApproved) {}

bool Voter::hasVotedInElection(const std::string& electionId) const {
    return votedElectionIds.find(electionId) != votedElectionIds.end();
}

void Voter::recordVote(const std::string& electionId) {
    votedElectionIds.insert(electionId);
}

std::string Voter::serialize() const {
    std::ostringstream ss;
    ss << id << "\t" << username << "\t" << passwordHash << "\t" << salt << "\t"
       << fullName << "\t" << nationalIdHash << "\t" << (isApproved ? "1" : "0") << "\t" << createdAt << "\t";
    
    bool first = true;
    for (const auto& eid : votedElectionIds) {
        if (!first) ss << ",";
        ss << eid;
        first = false;
    }
    return ss.str();
}

Voter Voter::deserializeVoter(const std::string& line) {
    std::stringstream ss(line);
    std::string id, username, passHash, salt, fullName, natIdHash, appStr, createdAt, votedStr;
    std::getline(ss, id, '\t');
    std::getline(ss, username, '\t');
    std::getline(ss, passHash, '\t');
    std::getline(ss, salt, '\t');
    std::getline(ss, fullName, '\t');
    std::getline(ss, natIdHash, '\t');
    std::getline(ss, appStr, '\t');
    std::getline(ss, createdAt, '\t');
    std::getline(ss, votedStr, '\t');

    bool approved = (appStr == "1");
    Voter v(id, username, passHash, salt, fullName, natIdHash, approved, createdAt);

    if (!votedStr.empty()) {
        std::stringstream vss(votedStr);
        std::string eid;
        while (std::getline(vss, eid, ',')) {
            if (!eid.empty()) v.recordVote(eid);
        }
    }
    return v;
}
