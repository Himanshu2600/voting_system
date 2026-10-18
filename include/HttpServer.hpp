#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "DataManager.hpp"
#include <string>
#include <functional>

class HttpServer {
private:
    int port;
    std::string webDir;
    DataManager& dataManager;
    bool running;

public:
    HttpServer(int port, std::string webDir, DataManager& dm);
    ~HttpServer();

    void start();
    void stop();

private:
    void handleClient(int clientSocket);
    std::string getMimeType(const std::string& path);
    std::string buildResponse(int statusCode, const std::string& contentType, const std::string& body);
    
    // REST API Handlers
    std::string handleApiElections();
    std::string handleApiVote(const std::string& requestBody);
    std::string handleApiLedger();
    std::string handleApiVerifyReceipt(const std::string& requestBody);
    std::string handleApiCreateElection(const std::string& requestBody);
    std::string handleApiToggleStatus(const std::string& requestBody);
};

#endif // HTTP_SERVER_HPP
