#ifndef VOTER_HPP
#define VOTER_HPP

#include "User.hpp"
#include <vector>
#include <set>

class Voter : public User {
private:
    std::string fullName;
    std::string nationalIdHash; // Hashed SSN/National ID for verification
    bool isApproved;
    std::set<std::string> votedElectionIds; // Track elections voted in

public:
    Voter();
    Voter(std::string id, std::string username, std::string passwordHash, std::string salt,
          std::string fullName, std::string nationalIdHash, bool isApproved, std::string createdAt);

    std::string getFullName() const { return fullName; }
    std::string getNationalIdHash() const { return nationalIdHash; }
    bool getIsApproved() const { return isApproved; }
    void setApproved(bool approved) { isApproved = approved; }

    bool hasVotedInElection(const std::string& electionId) const;
    void recordVote(const std::string& electionId);
    const std::set<std::string>& getVotedElectionIds() const { return votedElectionIds; }

    std::string serialize() const override;
    static Voter deserializeVoter(const std::string& line);
};

#endif // VOTER_HPP
