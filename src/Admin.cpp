#include "Admin.hpp"
#include <sstream>

Admin::Admin() : User(), adminCode("DEFAULT_ADMIN") {}

Admin::Admin(std::string id, std::string username, std::string passwordHash, std::string salt,
             std::string adminCode, std::string createdAt)
    : User(id, username, passwordHash, salt, UserRole::ADMIN, createdAt), adminCode(adminCode) {}

std::string Admin::serialize() const {
    std::ostringstream ss;
    ss << id << "\t" << username << "\t" << passwordHash << "\t" << salt << "\t"
       << adminCode << "\t" << createdAt;
    return ss.str();
}

Admin Admin::deserializeAdmin(const std::string& line) {
    std::stringstream ss(line);
    std::string id, username, passHash, salt, code, createdAt;
    std::getline(ss, id, '\t');
    std::getline(ss, username, '\t');
    std::getline(ss, passHash, '\t');
    std::getline(ss, salt, '\t');
    std::getline(ss, code, '\t');
    std::getline(ss, createdAt, '\t');

    return Admin(id, username, passHash, salt, code, createdAt);
}
