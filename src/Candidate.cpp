#include "Candidate.hpp"
#include <sstream>

Candidate::Candidate() : voteCount(0) {}

Candidate::Candidate(std::string id, std::string electionId, std::string name, std::string party, std::string manifesto, int voteCount)
    : id(id), electionId(electionId), name(name), party(party), manifesto(manifesto), voteCount(voteCount) {}

std::string Candidate::serialize() const {
    std::ostringstream ss;
    ss << id << "\t" << electionId << "\t" << name << "\t" << party << "\t" << manifesto << "\t" << voteCount;
    return ss.str();
}

Candidate Candidate::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string id, electionId, name, party, manifesto, vcStr;
    std::getline(ss, id, '\t');
    std::getline(ss, electionId, '\t');
    std::getline(ss, name, '\t');
    std::getline(ss, party, '\t');
    std::getline(ss, manifesto, '\t');
    std::getline(ss, vcStr, '\t');

    int count = vcStr.empty() ? 0 : std::stoi(vcStr);
    return Candidate(id, electionId, name, party, manifesto, count);
}
