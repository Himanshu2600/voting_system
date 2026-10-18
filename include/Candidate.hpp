#ifndef CANDIDATE_HPP
#define CANDIDATE_HPP

#include <string>

class Candidate {
private:
    std::string id;
    std::string electionId;
    std::string name;
    std::string party;
    std::string manifesto;
    int voteCount;

public:
    Candidate();
    Candidate(std::string id, std::string electionId, std::string name, std::string party, std::string manifesto, int voteCount = 0);

    std::string getId() const { return id; }
    std::string getElectionId() const { return electionId; }
    std::string getName() const { return name; }
    std::string getParty() const { return party; }
    std::string getManifesto() const { return manifesto; }
    int getVoteCount() const { return voteCount; }

    void incrementVotes() { voteCount++; }

    std::string serialize() const;
    static Candidate deserialize(const std::string& line);
};

#endif // CANDIDATE_HPP
