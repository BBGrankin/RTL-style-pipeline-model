#pragma once

#include "samples.h"
#include <vector>
#include "errors.h"

void print_both_models(const std::vector<OutputSample> &comb, 
                       const std::vector<OutputSample> &pipe, 
                       const std::vector<InputSample> &sample,
                       std::size_t latency);

void error_finding(const std::vector<OutputSample> &comb, 
                   const std::vector<OutputSample> &pipe, 
                   const std::vector<InputSample> &sample,
                   ErrorsLog &errors, std::size_t latency);

void model_comparison_latency_1(const std::vector<InputSample> &sample);

void model_comparison_latency_2(const std::vector<InputSample> &sample);