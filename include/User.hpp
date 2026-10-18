#ifndef USER_HPP
#define USER_HPP

#include <string>

enum class UserRole {
    VOTER,
    ADMIN
};

class User {
protected:
    std::string id;
    std::string username;
    std::string passwordHash;
    std::string salt;
    UserRole role;
    std::string createdAt;

public:
    User();
    User(std::string id, std::string username, std::string passwordHash, std::string salt, UserRole role, std::string createdAt);
    virtual ~User() = default;

    std::string getId() const { return id; }
    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }
    std::string getSalt() const { return salt; }
    UserRole getRole() const { return role; }
    std::string getRoleString() const { return role == UserRole::ADMIN ? "ADMIN" : "VOTER"; }
    std::string getCreatedAt() const { return createdAt; }

    bool verifyPassword(const std::string& inputPassword) const;
    void setPassword(const std::string& newPassword);

    virtual std::string serialize() const;
    static User deserialize(const std::string& line);
};

#endif // USER_HPP
