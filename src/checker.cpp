#include "checker.h"
#include "models.h"
#include <iostream>
#include "formatting.h"

void print_both_models(const std::vector<OutputSample> &comb, 
                       const std::vector<OutputSample> &pipe, 
                       const std::vector<InputSample> &sample,
                       std::size_t latency){
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
    for (std::size_t i {latency}; i > 0; --i){
        std::cout << "cycle " << pipe.size() - i << separator << "  0  " 
        << separator << "  0  " << separator << 0 
        << separator << 0 << separator << 0
        << separator << "    0     " << separator 
        << "  0   " << separator << "    " << pipe[pipe.size() - i].valid
        << "     " << separator << "  " 
        << pipe[pipe.size() - i].y << "   " << separator << '\n' << line;
    }
}

void error_finding(const std::vector<OutputSample> &comb, 
                   const std::vector<OutputSample> &pipe, 
                   const std::vector<InputSample> &sample,
                   ErrorsLog &errors, std::size_t latency){
    for (std::size_t i {}; i < comb.size(); ++i){
        OutputSample expected {comb[i]};
        bool reset_on_path_occured {false};
        for (std::size_t j {1}; j <= latency; ++j){
            if (i + j < sample.size() && sample[i + j].reset == 1){
                expected.valid = 0;
                expected.y = 0;
                reset_on_path_occured = true;
                break;
            }
        }
        if (expected.y != pipe[i + latency].y || 
            expected.valid != pipe[i + latency].valid ){
            Error error;    
            error.input_cycle = i;
            error.pipe_cycle = i + latency;
            error.real_valid = pipe[i + latency].valid;
            error.real_y = pipe[i + latency].y;
            error.reset = reset_on_path_occured;
            error.expected_valid = expected.valid;
            error.expected_y = expected.y;
            errors.push_back(error);
        } 
    }
}

void model_comparison_latency_1(const std::vector<InputSample> &sample){
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
    errors.setLatency(1);
    error_finding(comb_outputs, pipe_outputs, sample, errors, 1);
    print_both_models(comb_outputs, pipe_outputs, sample, 1);
    if (errors.size() == 0) std::cout << "\nCheck: OK\n\n";
    else{
        std::cout << "\nCheck: FAILED\n\n";
        if (errors.size() > 5) errors.print_first();
        else errors.print_all();
    }
}

void model_comparison_latency_2(const std::vector<InputSample> &sample){
    CombinationModel comb;
    TwoStagePipelineModel pipe;
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
    pipe_outputs.push_back(pipe.tick(0, 0, 0, 0, 0));
    ErrorsLog errors;
    errors.setLatency(2);
    error_finding(comb_outputs, pipe_outputs, sample, errors, 2);
    print_both_models(comb_outputs, pipe_outputs, sample, 2);
    if (errors.size() == 0) std::cout << "\nCheck: OK\n\n";
    else{
        std::cout << "\nCheck: FAILED\n\n";
        if (errors.size() > 5) errors.print_first();
        else errors.print_all();
    }
}