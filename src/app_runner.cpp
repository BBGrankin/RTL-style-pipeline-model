#include "app_runner.h"
#include "file_reader.h"
#include "checker.h"
#include <string>
#include <iostream>

void run_comparison(int argc, char** argv){
    if (argc == 1){
        model_comparison_latency_1(read_input_trace("input.txt"));
    }
    else if (argc == 2){
        model_comparison_latency_1(read_input_trace(argv[1]));
    }
    else if (argc == 3){
        int latency {};
        try {
            latency = (std::stoi(argv[2]));
        }
        catch(const std::exception& e) {
            throw std::invalid_argument("\nWrong third parameter\n");
        }
        if (latency == 1){
            model_comparison_latency_1(read_input_trace(argv[1]));
        }
        else if (latency == 2){
            model_comparison_latency_2(read_input_trace(argv[1]));
        }
        else throw std::invalid_argument("\nThird parameter should be "
                                            "in range from 1 to 2\n");  
    }
    else if (argc == 4){
        int latency {};
        try {
            latency = (std::stoi(argv[3]));
        }
        catch(const std::exception&) {
            throw std::invalid_argument("\nWrong latency parameter\n");
        }
        comparison_of_input_output_models(argv[1], argv[2], latency);
    }
    else {
        throw std::invalid_argument("\nWrong number of files\n");
    }
}