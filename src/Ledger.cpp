#include "Ledger.hpp"
#include "CryptoUtils.hpp"
#include <sstream>

std::string Block::calculateHash() const {
    std::string rawData = std::to_string(index) + "|" + timestamp + "|" + vote.getPayloadString() + "|" + previousHash;
    return CryptoUtils::sha256(rawData);
}

std::string Block::serialize() const {
    std::ostringstream ss;
    ss << index << "\t" << timestamp << "\t" << previousHash << "\t" << blockHash << "\t" << vote.serialize();
    return ss.str();
}

Block Block::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string idxStr, ts, prevHash, bHash, voteSerialized;
    std::getline(ss, idxStr, '\t');
    std::getline(ss, ts, '\t');
    std::getline(ss, prevHash, '\t');
    std::getline(ss, bHash, '\t');
    std::getline(ss, voteSerialized);

    Block b;
    b.index = idxStr.empty() ? 0 : std::stoull(idxStr);
    b.timestamp = ts;
    b.previousHash = prevHash;
    b.blockHash = bHash;
    if (!voteSerialized.empty()) {
        b.vote = Vote::deserialize(voteSerialized);
    }
    return b;
}

Ledger::Ledger() {
    // Chain will either load from disk or create genesis block
}

void Ledger::createGenesisBlock() {
    chain.clear();
    Block genesis;
    genesis.index = 0;
    genesis.timestamp = CryptoUtils::getCurrentTimestamp();
    genesis.vote = Vote("GENESIS", "SYSTEM", "NONE", "GENESIS_RECEIPT", genesis.timestamp);
    genesis.previousHash = "0000000000000000000000000000000000000000000000000000000000000000";
    genesis.blockHash = genesis.calculateHash();
    chain.push_back(genesis);
}

Block Ledger::addVote(const Vote& vote) {
    if (chain.empty()) {
        createGenesisBlock();
    }

    Block newBlock;
    newBlock.index = chain.size();
    newBlock.timestamp = vote.getTimestamp();
    newBlock.vote = vote;
    newBlock.previousHash = chain.back().blockHash;
    newBlock.blockHash = newBlock.calculateHash();

    chain.push_back(newBlock);
    return newBlock;
}

bool Ledger::verifyIntegrity(std::vector<std::string>& auditLog) const {
    auditLog.clear();
    if (chain.empty()) {
        auditLog.push_back("[WARNING] Ledger is empty.");
        return true;
    }

    bool isValid = true;

    for (size_t i = 0; i < chain.size(); ++i) {
        const Block& current = chain[i];

        // 1. Recalculate block hash
        std::string recalculated = current.calculateHash();
        if (recalculated != current.blockHash) {
            auditLog.push_back("[CORRUPTED] Block #" + std::to_string(current.index) +
                               " hash mismatch! Stored: " + current.blockHash.substr(0, 10) +
                               "... Computed: " + recalculated.substr(0, 10) + "...");
            isValid = false;
        }

        // 2. Verify link to previous block
        if (i > 0) {
            const Block& previous = chain[i - 1];
            if (current.previousHash != previous.blockHash) {
                auditLog.push_back("[CORRUPTED] Block #" + std::to_string(current.index) +
                                   " previousHash does not match Block #" + std::to_string(previous.index) + " blockHash!");
                isValid = false;
            }
        }
    }

    if (isValid) {
        auditLog.push_back("[SUCCESS] Cryptographic ledger integrity verified. " +
                           std::to_string(chain.size()) + " blocks intact and untampered.");
    }
    return isValid;
}

const Block* Ledger::findVoteByReceipt(const std::string& receiptCode) const {
    for (const auto& block : chain) {
        if (block.vote.getVoterReceiptCode() == receiptCode) {
            return &block;
        }
    }
    return nullptr;
}

std::string Ledger::serialize() const {
    std::ostringstream ss;
    for (const auto& block : chain) {
        ss << block.serialize() << "\n";
    }
    return ss.str();
}

void Ledger::loadFromSerialized(const std::vector<std::string>& lines) {
    chain.clear();
    for (const auto& line : lines) {
        if (!line.empty()) {
            chain.push_back(Block::deserialize(line));
        }
    }
    if (chain.empty()) {
        createGenesisBlock();
    }
}
