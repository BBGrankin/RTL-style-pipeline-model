#include "errors.h"
#include <iostream>
#include "samples.h"
#include "formatting.h"

std::size_t ErrorsLog::size() const{
    return errors.size();
}

void ErrorsLog::push_back(const Error &error){
    errors.push_back(error);
}

void ErrorsLog::print_all() const {
    for (std::size_t i {}; i < errors.size(); ++i){
        std::cout << line << "Error in cycle - " 
        << errors[i].input_cycle
        << "\tOn pipe cycle - " << errors[i].pipe_cycle
        << "\tExpected valid - " << errors[i].expected_valid
        << " | y - " << errors[i].expected_y
        << "\tActual valid - " << errors[i].real_valid
        << " | y - " << errors[i].real_y 
        << "\tReset occurred - " << std::boolalpha 
        << errors[i].reset << '\n';
        if (errors[i].reset)
            std::cout << "\nReason: expected invalid output because "
                            "reset is active on output cycle\n"
            << line << '\n';
        else
            std::cout << "\nReason: pipeline output differs "
                            "from combinational output with latency "
            << latency << '\n' << line << '\n';
    }
}

void ErrorsLog::print_first() const{
    if (errors.size() == 0) return;
    std::cout << line << "Error in cycle - " 
    << errors[0].input_cycle
    << "\tOn pipe cycle - " << errors[0].pipe_cycle
    << "\tExpected valid - " << errors[0].expected_valid
    << " | y - " << errors[0].expected_y
    << "\tActual valid - " << errors[0].real_valid
    << " | y - " << errors[0].real_y 
    << "\tReset occurred - " << std::boolalpha 
    << errors[0].reset << '\n';
    if (errors[0].reset)
        std::cout << "\nReason: expected invalid output because "
                        "reset is active on output cycle\n"
        << line << '\n';
    else
        std::cout << "\nReason: pipeline output differs "
                        "from combinational output with latency "
        << latency << '\n' << line << '\n';     
}