#include <iostream>
#include "app_runner.h"
#include "formatting.h"

int main(int argc, char **argv){
    std::cout << '\n' << line << line;
    try {
        run_comparison(argc, argv);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    std::cout << line << line << '\n';
    return 0;
}