#ifndef CRYPTO_UTILS_HPP
#define CRYPTO_UTILS_HPP

#include <string>
#include <vector>
#include <cstdint>

class CryptoUtils {
public:
    // Computes SHA-256 hash of a string input and returns 64-char hex string
    static std::string sha256(const std::string& input);

    // Generates a random salt string
    static std::string generateSalt(size_t length = 16);

    // Hashes password with salt: SHA256(password + salt)
    static std::string hashPassword(const std::string& password, const std::string& salt);

    // Generates voter receipt code (e.g. RCP-8F92A1...)
    static std::string generateReceiptCode(const std::string& voterId, const std::string& electionId, const std::string& timestamp);

    // Formats current time as ISO timestamp string
    static std::string getCurrentTimestamp();
};

#endif // CRYPTO_UTILS_HPP
