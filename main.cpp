#include <vector>
#include "samples.h"
#include "checker.h"
#include <iostream>
#include "trace_reader.h"

int main(int argc, char **argv){
    try {
        if (argc == 1){
            model_comparison(read_input_trace("input.txt"));
        }
        else if (argc == 2){
            model_comparison(read_input_trace(argv[1]));
        }
        else {
            throw "\nWrong number of files\n";
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}