#include "DataManager.hpp"
#include "HttpServer.hpp"
#include <iostream>

int main() {
    try {
        DataManager dm("./data");
        dm.loadAll();

        std::string webDir = "../online_voting_website";
        int port = 8080;

        HttpServer server(port, webDir, dm);
        server.start();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal Web Server Exception: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
