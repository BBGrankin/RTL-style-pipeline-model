#pragma once

#include "samples.h"
#include <vector>
#include "errors.h"
#include <string>

void print_both_models(const std::vector<OutputSample> &comb, 
                       const std::vector<OutputSample> &pipe, 
                       const std::vector<InputSample> &sample,
                       std::size_t latency);

void error_finding(const std::vector<OutputSample> &comb, 
                   const std::vector<OutputSample> &pipe, 
                   const std::vector<InputSample> &sample,
                   ErrorsLog &errors);

void model_comparison_latency_1(const std::vector<InputSample> &sample);

void model_comparison_latency_2(const std::vector<InputSample> &sample);

void comparison_of_input_output_models(const std::string& input_file,
                                       const std::string& output_file,
                                       int latency);

void comparison_of_two_latency_models(const std::vector<InputSample> &sample);