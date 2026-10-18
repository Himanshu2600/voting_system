#ifndef VOTE_HPP
#define VOTE_HPP

#include <string>

class Vote {
private:
    std::string voteId;
    std::string electionId;
    std::string candidateId;
    std::string voterReceiptCode;
    std::string timestamp;

public:
    Vote();
    Vote(std::string voteId, std::string electionId, std::string candidateId, std::string voterReceiptCode, std::string timestamp);

    std::string getVoteId() const { return voteId; }
    std::string getElectionId() const { return electionId; }
    std::string getCandidateId() const { return candidateId; }
    std::string getVoterReceiptCode() const { return voterReceiptCode; }
    std::string getTimestamp() const { return timestamp; }

    std::string getPayloadString() const;

    std::string serialize() const;
    static Vote deserialize(const std::string& line);
};

#endif // VOTE_HPP
