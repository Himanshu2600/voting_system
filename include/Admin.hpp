#ifndef ADMIN_HPP
#define ADMIN_HPP

#include "User.hpp"

class Admin : public User {
private:
    std::string adminCode;

public:
    Admin();
    Admin(std::string id, std::string username, std::string passwordHash, std::string salt,
          std::string adminCode, std::string createdAt);

    std::string getAdminCode() const { return adminCode; }

    std::string serialize() const override;
    static Admin deserializeAdmin(const std::string& line);
};

#endif // ADMIN_HPP
