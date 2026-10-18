#ifndef LEDGER_HPP
#define LEDGER_HPP

#include "Vote.hpp"
#include <vector>
#include <string>

struct Block {
    size_t index;
    std::string timestamp;
    Vote vote;
    std::string previousHash;
    std::string blockHash;

    std::string calculateHash() const;
    std::string serialize() const;
    static Block deserialize(const std::string& line);
};

class Ledger {
private:
    std::vector<Block> chain;

public:
    Ledger();

    void createGenesisBlock();
    Block addVote(const Vote& vote);

    const std::vector<Block>& getChain() const { return chain; }
    size_t getSize() const { return chain.size(); }

    bool verifyIntegrity(std::vector<std::string>& auditLog) const;
    const Block* findVoteByReceipt(const std::string& receiptCode) const;

    std::string serialize() const;
    void loadFromSerialized(const std::vector<std::string>& lines);
};

#endif // LEDGER_HPP
