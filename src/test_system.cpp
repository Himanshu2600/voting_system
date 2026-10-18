#include "DataManager.hpp"
#include "CryptoUtils.hpp"
#include <iostream>
#include <cassert>
#include <vector>

void runTests() {
    std::cout << "========================================================\n";
    std::cout << "         RUNNING ONLINE VOTING SYSTEM TEST SUITE        \n";
    std::cout << "========================================================\n";

    // Initialize test DataManager using a test directory
    DataManager dm("./test_data");
    dm.loadAll();

    // 1. Test Seed Data
    std::cout << "[TEST 1] Verifying Initial Seed Data Loading...\n";
    auto elections = dm.getAllElections();
    assert(!elections.empty());
    std::cout << " -> PASSED! Elections loaded: " << elections.size() << "\n";

    // 2. Test Voter Registration & Authentication
    std::cout << "[TEST 2] Testing Voter Registration & Authentication...\n";
    std::string msg;
    bool regOk = dm.registerVoter("testvoter", "securePass123", "Test User", "9999", msg);
    assert(regOk);

    User* user = dm.authenticate("testvoter", "securePass123");
    assert(user != nullptr);
    assert(user->getUsername() == "testvoter");
    std::cout << " -> PASSED! Voter registered and authenticated successfully.\n";

    // 3. Test Admin Registration
    std::cout << "[TEST 3] Testing Admin Registration...\n";
    bool adminRegOk = dm.registerAdmin("testadmin", "adminPass123", "ADMIN2026", msg);
    assert(adminRegOk);
    User* adminUser = dm.authenticate("testadmin", "adminPass123");
    assert(adminUser != nullptr);
    assert(adminUser->getRole() == UserRole::ADMIN);
    std::cout << " -> PASSED! Admin registered and authenticated.\n";

    // 4. Test Election Creation & Candidates
    std::cout << "[TEST 4] Testing Election Creation & Candidate Addition...\n";
    std::string newElecId;
    bool createOk = dm.createElection("Test Election 2026", "Unit Test Election", "2026-01-01", "2026-12-31", newElecId);
    assert(createOk);

    bool addCand1 = dm.addCandidate(newElecId, "Candidate Alpha", "Party A", "Platform A");
    bool addCand2 = dm.addCandidate(newElecId, "Candidate Beta", "Party B", "Platform B");
    assert(addCand1 && addCand2);
    auto cands = dm.getCandidatesForElection(newElecId);
    assert(cands.size() == 2);
    std::cout << " -> PASSED! Election " << newElecId << " created with 2 candidates.\n";

    // 5. Test Voting & Receipt Generation
    std::cout << "[TEST 5] Testing Vote Casting & Receipt Code...\n";
    std::string receiptCode;
    bool voteOk = dm.castVote("testvoter", newElecId, cands[0].getId(), receiptCode, msg);
    assert(voteOk);
    assert(receiptCode.rfind("RCP-", 0) == 0);
    std::cout << " -> PASSED! Vote cast successfully. Receipt Code: " << receiptCode << "\n";

    // 6. Test Double-Voting Prevention
    std::cout << "[TEST 6] Testing Double-Voting Prevention...\n";
    std::string receipt2;
    bool doubleVoteOk = dm.castVote("testvoter", newElecId, cands[1].getId(), receipt2, msg);
    assert(!doubleVoteOk); // Should fail!
    std::cout << " -> PASSED! Double-voting attempt rejected correctly.\n";

    // 7. Test Cryptographic Ledger Integrity
    std::cout << "[TEST 7] Testing Cryptographic Hash-Chain Ledger Audit...\n";
    std::vector<std::string> auditLog;
    bool ledgerValid = dm.verifyLedger(auditLog);
    assert(ledgerValid);
    std::cout << " -> PASSED! Cryptographic ledger hash chain intact.\n";

    // 8. Test Receipt Lookup
    std::cout << "[TEST 8] Testing Receipt Lookup in Ledger...\n";
    const Block* block = dm.verifyReceipt(receiptCode);
    assert(block != nullptr);
    assert(block->vote.getVoterReceiptCode() == receiptCode);
    std::cout << " -> PASSED! Receipt found in Block #" << block->index << " with hash: " << block->blockHash.substr(0, 16) << "...\n";

    std::cout << "\n========================================================\n";
    std::cout << "        ALL 8 UNIT & SYSTEM TESTS PASSED CLEANLY!       \n";
    std::cout << "========================================================\n";
}

int main() {
    try {
        runTests();
    } catch (const std::exception& ex) {
        std::cerr << "Test failed with exception: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
