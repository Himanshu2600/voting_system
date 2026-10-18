#include "Vote.hpp"
#include <sstream>

Vote::Vote() {}

Vote::Vote(std::string voteId, std::string electionId, std::string candidateId, std::string voterReceiptCode, std::string timestamp)
    : voteId(voteId), electionId(electionId), candidateId(candidateId), voterReceiptCode(voterReceiptCode), timestamp(timestamp) {}

std::string Vote::getPayloadString() const {
    return voteId + ":" + electionId + ":" + candidateId + ":" + voterReceiptCode + ":" + timestamp;
}

std::string Vote::serialize() const {
    std::ostringstream ss;
    ss << voteId << "\t" << electionId << "\t" << candidateId << "\t" << voterReceiptCode << "\t" << timestamp;
    return ss.str();
}

Vote Vote::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string vid, eid, cid, rcode, ts;
    std::getline(ss, vid, '\t');
    std::getline(ss, eid, '\t');
    std::getline(ss, cid, '\t');
    std::getline(ss, rcode, '\t');
    std::getline(ss, ts, '\t');

    return Vote(vid, eid, cid, rcode, ts);
}
