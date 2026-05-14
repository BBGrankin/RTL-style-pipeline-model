#include "checker.h"
#include "models.h"
#include <iostream>

void print_both_models(const std::vector<OutputSample> &comb, 
                       const std::vector<OutputSample> &pipe, 
                       const std::vector<InputSample> &sample){
    std::cout << "\ncycle  " << separator << "reset"
    << separator << "valid" << separator << 'a'
    << separator << 'b' << separator << 'c' << separator
    << "comb_valid" << separator << "comb_y" << separator
    << "pipe_valid" << separator << "pipe_y" << separator
    << '\n' << line;
    for (std::size_t i {}; i < comb.size(); ++i){
        std::cout << "cycle " << i << separator << "  " << sample[i].reset
        << "  " << separator << "  " << sample[i].valid
        << "  " << separator << sample[i].a << separator
        << sample[i].b << separator << sample[i].c 
        << separator << "    " << comb[i].valid << "     "
        << separator << "  " << comb[i].y << "   "
        << separator << "    " << pipe[i].valid << "     " 
        << separator << "  " << pipe[i].y << "   " 
        << separator << '\n' << line;
    }
    std::cout << "cycle " << pipe.size() - 1 << separator << "  0  " 
    << separator << "  0  " << separator << 0 
    << separator << 0 << separator << 0
    << separator << "    0     " << separator 
    << "  0   " << separator << "    " << pipe.back().valid
    << "     " << separator << "  " 
    << pipe.back().y << "   " << separator << '\n' << line; 
}

void error_finding(const std::vector<OutputSample> &comb, 
                   const std::vector<OutputSample> &pipe, 
                   const std::vector<InputSample> &sample,
                   ErrorsLog &errors){
    for (std::size_t i {}; i < comb.size(); ++i){
        OutputSample expected {};
        if (i + 1 < sample.size() && sample[i + 1].reset == 1){
            expected.valid = 0;
            expected.y = 0;
        }
        else{
            expected = comb[i];
        }
        if (expected.y != pipe[i + 1].y || 
            expected.valid != pipe[i + 1].valid ){
            Error error;    
            error.input_cycle = i;
            error.pipe_cycle = i + 1;
            error.real_valid = pipe[i + 1].valid;
            error.real_y = pipe[i + 1].y;
            if (i + 1 < sample.size())
                error.reset = sample[i + 1].reset;
            else
                error.reset = 0;
            error.expected_valid = expected.valid;
            error.expected_y = expected.y;
            errors.push_back(error);
        } 
    }
}

void model_comparison(const std::vector<InputSample> &sample){
    CombinationModel comb;
    PipelinedModel pipe;
    std::vector<OutputSample> comb_outputs, pipe_outputs;
    for (std::size_t i {}; i < sample.size(); ++i){
        comb_outputs.push_back(comb.tick(sample[i].reset, sample[i].valid, 
                                         sample[i].a, sample[i].b, 
                                         sample[i].c));
        pipe_outputs.push_back(pipe.tick(sample[i].reset, sample[i].valid, 
                                         sample[i].a, 
                                         sample[i].b, 
                                         sample[i].c));
    }
    pipe_outputs.push_back(pipe.tick(0, 0, 0, 0, 0));
    ErrorsLog errors;
    error_finding(comb_outputs, pipe_outputs, sample, errors);
    print_both_models(comb_outputs, pipe_outputs, sample);
    if (errors.size() == 0) std::cout << "\nCheck: OK\n\n";
    else{
        std::cout << "\nCheck: FAILED\n\n";
        if (errors.size() > 5) errors.print_first();
        else errors.print_all();
    }
}