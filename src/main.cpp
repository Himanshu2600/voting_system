#include "DataManager.hpp"
#include "ConsoleUI.hpp"
#include <iostream>

int main() {
    try {
        DataManager dm("./data");
        ConsoleUI ui(dm);
        ui.start();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal exception encountered: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
