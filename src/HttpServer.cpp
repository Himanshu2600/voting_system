#include "HttpServer.hpp"
#include "CryptoUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define CLOSE_SOCKET(s) close(s)
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

HttpServer::HttpServer(int port, std::string webDir, DataManager& dm)
    : port(port), webDir(webDir), dataManager(dm), running(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::stop() {
    running = false;
}

std::string HttpServer::getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".json") != std::string::npos) return "application/json";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".svg") != std::string::npos) return "image/svg+xml";
    return "text/plain";
}

std::string HttpServer::buildResponse(int statusCode, const std::string& contentType, const std::string& body) {
    std::stringstream ss;
    std::string statusStr = (statusCode == 200) ? "200 OK" : (statusCode == 404 ? "404 Not Found" : "400 Bad Request");
    ss << "HTTP/1.1 " << statusStr << "\r\n";
    ss << "Content-Type: " << contentType << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Access-Control-Allow-Headers: Content-Type\r\n";
    ss << "Connection: close\r\n\r\n";
    ss << body;
    return ss.str();
}

// Simple JSON extraction helper
static std::string extractJsonValue(const std::string& json, const std::string& key) {
    std::size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";
    std::size_t colon = json.find(":", pos);
    if (colon == std::string::npos) return "";
    std::size_t startQuote = json.find("\"", colon);
    if (startQuote == std::string::npos) return "";
    std::size_t endQuote = json.find("\"", startQuote + 1);
    if (endQuote == std::string::npos) return "";
    return json.substr(startQuote + 1, endQuote - startQuote - 1);
}

std::string HttpServer::handleApiElections() {
    auto elections = dataManager.getAllElections();
    std::stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < elections.size(); ++i) {
        const auto& e = elections[i];
        ss << "  {\n";
        ss << "    \"id\": \"" << e.getId() << "\",\n";
        ss << "    \"title\": \"" << e.getTitle() << "\",\n";
        ss << "    \"description\": \"" << e.getDescription() << "\",\n";
        ss << "    \"status\": \"" << e.getStatusString() << "\",\n";
        ss << "    \"totalVotes\": " << e.getTotalVotesCast() << ",\n";
        ss << "    \"candidates\": [\n";

        auto candidates = dataManager.getCandidatesForElection(e.getId());
        for (size_t j = 0; j < candidates.size(); ++j) {
            const auto& c = candidates[j];
            ss << "      {\n";
            ss << "        \"id\": \"" << c.getId() << "\",\n";
            ss << "        \"name\": \"" << c.getName() << "\",\n";
            ss << "        \"party\": \"" << c.getParty() << "\",\n";
            ss << "        \"manifesto\": \"" << c.getManifesto() << "\",\n";
            ss << "        \"votes\": " << c.getVoteCount() << "\n";
            ss << "      }" << (j + 1 < candidates.size() ? "," : "") << "\n";
        }
        ss << "    ]\n";
        ss << "  }" << (i + 1 < elections.size() ? "," : "") << "\n";
    }
    ss << "]";
    return buildResponse(200, "application/json", ss.str());
}

std::string HttpServer::handleApiVote(const std::string& requestBody) {
    std::string username = extractJsonValue(requestBody, "username");
    std::string electionId = extractJsonValue(requestBody, "electionId");
    std::string candidateId = extractJsonValue(requestBody, "candidateId");

    if (username.empty()) username = "voter1"; // Default voter for web demo if unassigned

    std::string receiptCode, msg;
    bool ok = dataManager.castVote(username, electionId, candidateId, receiptCode, msg);

    std::stringstream ss;
    if (ok) {
        ss << "{\n";
        ss << "  \"success\": true,\n";
        ss << "  \"receiptCode\": \"" << receiptCode << "\",\n";
        ss << "  \"message\": \"" << msg << "\"\n";
        ss << "}";
        return buildResponse(200, "application/json", ss.str());
    } else {
        ss << "{\n";
        ss << "  \"success\": false,\n";
        ss << "  \"message\": \"" << msg << "\"\n";
        ss << "}";
        return buildResponse(400, "application/json", ss.str());
    }
}

std::string HttpServer::handleApiLedger() {
    const auto& chain = dataManager.getLedger().getChain();
    std::stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < chain.size(); ++i) {
        const auto& block = chain[i];
        ss << "  {\n";
        ss << "    \"index\": " << block.index << ",\n";
        ss << "    \"timestamp\": \"" << block.timestamp << "\",\n";
        ss << "    \"electionId\": \"" << block.vote.getElectionId() << "\",\n";
        ss << "    \"receiptCode\": \"" << block.vote.getVoterReceiptCode() << "\",\n";
        ss << "    \"previousHash\": \"" << block.previousHash << "\",\n";
        ss << "    \"blockHash\": \"" << block.blockHash << "\"\n";
        ss << "  }" << (i + 1 < chain.size() ? "," : "") << "\n";
    }
    ss << "]";
    return buildResponse(200, "application/json", ss.str());
}

std::string HttpServer::handleApiVerifyReceipt(const std::string& requestBody) {
    std::string receipt = extractJsonValue(requestBody, "receiptCode");
    const Block* block = dataManager.verifyReceipt(receipt);

    std::stringstream ss;
    if (block) {
        ss << "{\n";
        ss << "  \"found\": true,\n";
        ss << "  \"index\": " << block->index << ",\n";
        ss << "  \"timestamp\": \"" << block->timestamp << "\",\n";
        ss << "  \"electionId\": \"" << block->vote.getElectionId() << "\",\n";
        ss << "  \"blockHash\": \"" << block->blockHash << "\",\n";
        ss << "  \"previousHash\": \"" << block->previousHash << "\"\n";
        ss << "}";
    } else {
        ss << "{\"found\": false, \"message\": \"Receipt code not found in ledger.\"}";
    }
    return buildResponse(200, "application/json", ss.str());
}

std::string HttpServer::handleApiCreateElection(const std::string& requestBody) {
    std::string title = extractJsonValue(requestBody, "title");
    std::string desc = extractJsonValue(requestBody, "description");
    std::string startDate = extractJsonValue(requestBody, "startDate");
    std::string endDate = extractJsonValue(requestBody, "endDate");

    if (title.empty()) title = "New Election";
    if (startDate.empty()) startDate = "2026-08-01";
    if (endDate.empty()) endDate = "2026-09-01";

    std::string outId;
    dataManager.createElection(title, desc, startDate, endDate, outId);

    std::stringstream ss;
    ss << "{\"success\": true, \"electionId\": \"" << outId << "\"}";
    return buildResponse(200, "application/json", ss.str());
}

std::string HttpServer::handleApiToggleStatus(const std::string& requestBody) {
    std::string elecId = extractJsonValue(requestBody, "electionId");
    std::string statusStr = extractJsonValue(requestBody, "status");

    ElectionStatus st = ElectionStatus::ACTIVE;
    if (statusStr == "CLOSED") st = ElectionStatus::CLOSED;
    else if (statusStr == "UPCOMING") st = ElectionStatus::UPCOMING;

    bool ok = dataManager.updateElectionStatus(elecId, st);
    std::stringstream ss;
    ss << "{\"success\": " << (ok ? "true" : "false") << "}";
    return buildResponse(200, "application/json", ss.str());
}

void HttpServer::handleClient(int clientSocket) {
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        CLOSE_SOCKET(clientSocket);
        return;
    }

    std::string request(buffer);
    std::stringstream reqStream(request);
    std::string method, path, protocol;
    reqStream >> method >> path >> protocol;

    // Extract body if POST request
    std::string body = "";
    std::size_t bodyPos = request.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        body = request.substr(bodyPos + 4);
    }

    std::string response;

    // API Routes
    if (path == "/api/elections" && method == "GET") {
        response = handleApiElections();
    } else if (path == "/api/vote" && method == "POST") {
        response = handleApiVote(body);
    } else if (path == "/api/ledger" && method == "GET") {
        response = handleApiLedger();
    } else if (path == "/api/verify_receipt" && method == "POST") {
        response = handleApiVerifyReceipt(body);
    } else if (path == "/api/admin/create_election" && method == "POST") {
        response = handleApiCreateElection(body);
    } else if (path == "/api/admin/toggle_status" && method == "POST") {
        response = handleApiToggleStatus(body);
    } 
    // Static File Serving
    else {
        if (path == "/") path = "/index.html";
        std::string filePath = webDir + path;

        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            response = buildResponse(200, getMimeType(path), buffer.str());
        } else {
            response = buildResponse(404, "text/plain", "404 Not Found");
        }
    }

    send(clientSocket, response.c_str(), response.size(), 0);
    CLOSE_SOCKET(clientSocket);
}

void HttpServer::start() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        return;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed on port " << port << ".\n";
        CLOSE_SOCKET(serverFd);
        return;
    }

    if (listen(serverFd, 10) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        CLOSE_SOCKET(serverFd);
        return;
    }

    running = true;
    std::cout << "\n========================================================================\n";
    std::cout << " 🌐 C++ WEB SERVER RUNNING FOR ONLINE VOTING SYSTEM                      \n";
    std::cout << "========================================================================\n";
    std::cout << " >> Web App URL : http://localhost:" << port << "\n";
    std::cout << " >> Data Store  : Loaded & Ready\n";
    std::cout << " >> C++ REST API: Active on http://localhost:" << port << "/api/\n";
    std::cout << "========================================================================\n\n";

    while (running) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);

        if (clientSocket != INVALID_SOCKET) {
            std::thread(&HttpServer::handleClient, this, clientSocket).detach();
        }
    }

    CLOSE_SOCKET(serverFd);
#ifdef _WIN32
    WSACleanup();
#endif
}
