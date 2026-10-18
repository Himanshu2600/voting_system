#include "User.hpp"
#include "CryptoUtils.hpp"
#include <sstream>

User::User() : role(UserRole::VOTER) {}

User::User(std::string id, std::string username, std::string passwordHash, std::string salt, UserRole role, std::string createdAt)
    : id(id), username(username), passwordHash(passwordHash), salt(salt), role(role), createdAt(createdAt) {}

bool User::verifyPassword(const std::string& inputPassword) const {
    std::string hashedInput = CryptoUtils::hashPassword(inputPassword, salt);
    return hashedInput == passwordHash;
}

void User::setPassword(const std::string& newPassword) {
    this->salt = CryptoUtils::generateSalt(16);
    this->passwordHash = CryptoUtils::hashPassword(newPassword, salt);
}

std::string User::serialize() const {
    std::ostringstream ss;
    ss << id << "\t" << username << "\t" << passwordHash << "\t" << salt << "\t" << getRoleString() << "\t" << createdAt;
    return ss.str();
}

User User::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string id, username, passHash, salt, roleStr, createdAt;
    std::getline(ss, id, '\t');
    std::getline(ss, username, '\t');
    std::getline(ss, passHash, '\t');
    std::getline(ss, salt, '\t');
    std::getline(ss, roleStr, '\t');
    std::getline(ss, createdAt, '\t');

    UserRole role = (roleStr == "ADMIN") ? UserRole::ADMIN : UserRole::VOTER;
    return User(id, username, passHash, salt, role, createdAt);
}
