#include <vector>
#include "samples.h"
#include "checker.h"
#include <iostream>
#include "trace_reader.h"

int main(){
    try {
        model_comparison(read_input_trace("input.txt"));
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}