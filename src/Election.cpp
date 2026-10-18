#include "Election.hpp"
#include <sstream>

Election::Election() : status(ElectionStatus::UPCOMING), totalVotesCast(0) {}

Election::Election(std::string id, std::string title, std::string description, ElectionStatus status,
                   std::string startDate, std::string endDate, int totalVotesCast)
    : id(id), title(title), description(description), status(status),
      startDate(startDate), endDate(endDate), totalVotesCast(totalVotesCast) {}

std::string Election::getStatusString() const {
    switch (status) {
        case ElectionStatus::UPCOMING: return "UPCOMING";
        case ElectionStatus::ACTIVE: return "ACTIVE";
        case ElectionStatus::CLOSED: return "CLOSED";
    }
    return "UNKNOWN";
}

std::string Election::serialize() const {
    std::ostringstream ss;
    ss << id << "\t" << title << "\t" << description << "\t" << getStatusString() << "\t"
       << startDate << "\t" << endDate << "\t" << totalVotesCast;
    return ss.str();
}

Election Election::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string id, title, desc, statusStr, sDate, eDate, tvCastStr;
    std::getline(ss, id, '\t');
    std::getline(ss, title, '\t');
    std::getline(ss, desc, '\t');
    std::getline(ss, statusStr, '\t');
    std::getline(ss, sDate, '\t');
    std::getline(ss, eDate, '\t');
    std::getline(ss, tvCastStr, '\t');

    ElectionStatus st = ElectionStatus::UPCOMING;
    if (statusStr == "ACTIVE") st = ElectionStatus::ACTIVE;
    else if (statusStr == "CLOSED") st = ElectionStatus::CLOSED;

    int totalVotes = tvCastStr.empty() ? 0 : std::stoi(tvCastStr);
    return Election(id, title, desc, st, sDate, eDate, totalVotes);
}
