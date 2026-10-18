#ifndef ELECTION_HPP
#define ELECTION_HPP

#include <string>
#include <vector>
#include "Candidate.hpp"

enum class ElectionStatus {
    UPCOMING,
    ACTIVE,
    CLOSED
};

class Election {
private:
    std::string id;
    std::string title;
    std::string description;
    ElectionStatus status;
    std::string startDate;
    std::string endDate;
    int totalVotesCast;

public:
    Election();
    Election(std::string id, std::string title, std::string description, ElectionStatus status,
             std::string startDate, std::string endDate, int totalVotesCast = 0);

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    ElectionStatus getStatus() const { return status; }
    std::string getStatusString() const;
    std::string getStartDate() const { return startDate; }
    std::string getEndDate() const { return endDate; }
    int getTotalVotesCast() const { return totalVotesCast; }

    void setStatus(ElectionStatus newStatus) { status = newStatus; }
    void incrementTotalVotes() { totalVotesCast++; }

    std::string serialize() const;
    static Election deserialize(const std::string& line);
};

#endif // ELECTION_HPP
