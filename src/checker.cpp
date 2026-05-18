#include "checker.h"
#include "models.h"
#include <iostream>
#include "formatting.h"
#include <format>
#include <iomanip>
#include "file_reader.h"

struct ExternalComparisonRow{
    std::string result;
    std::size_t cycle;
    int expected_valid;
    int expected_y;
    int actual_valid;
    int actual_y;
};

void print_both_models(const std::vector<OutputSample> &comb, 
                       const std::vector<OutputSample> &pipe, 
                       const std::vector<InputSample> &sample,
                       std::size_t latency){
    std::cout << '\n' << line << std::format("{:^{}}", "cycle", width) << '|'
    << std::format("{:^{}}", "reset", width) << '|'
    << std::format("{:^{}}", "valid", width) << '|'
    << std::format("{:^{}}", 'a', width) << '|'
    << std::format("{:^{}}", 'b', width) << '|'
    << std::format("{:^{}}", 'c', width) << '|'
    << std::format("{:^{}}", "comb_valid", width) << '|'
    << std::format("{:^{}}", "comb_y", width) << '|'
    << std::format("{:^{}}", "pipe_valid", width) << '|'
    << std::format("{:^{}}", "pipe_y", width) << '|'
    << '\n' << line;
    for (std::size_t i {}; i < comb.size(); ++i){
        std::string cycle {"cycle "};
        cycle += std::to_string(i);
        std::cout << std::format("{:^{}}", cycle, width) << '|'
        << std::format("{:^{}}", sample[i].reset, width) << '|'
        << std::format("{:^{}}", sample[i].valid, width) << '|'
        << std::format("{:^{}}", sample[i].a, width) << '|'
        << std::format("{:^{}}", sample[i].b, width) << '|'
        << std::format("{:^{}}", sample[i].c, width) << '|'
        << std::format("{:^{}}", comb[i].valid, width) << '|'
        << std::format("{:^{}}", comb[i].y, width) << '|'
        << std::format("{:^{}}", pipe[i].valid, width) << '|'
        << std::format("{:^{}}", pipe[i].y, width) << '|'
        << '\n' << line;
    }
    for (std::size_t i {latency}; i > 0; --i){
        std::string cycle {"cycle "};
        cycle += std::to_string(pipe.size() - i);
        std::cout << std::format("{:^{}}", cycle, width) << '|' 
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", 0, width) << '|'
        << std::format("{:^{}}", pipe[pipe.size() - i].valid, width) << '|'
        << std::format("{:^{}}", pipe[pipe.size() - i].y, width) << '|'
        << '\n' << line;
    }
}

void error_finding(const std::vector<OutputSample> &comb, 
                   const std::vector<OutputSample> &pipe, 
                   const std::vector<InputSample> &sample,
                   ErrorsLog &errors){
    for (std::size_t i {}; i < comb.size(); ++i){
        OutputSample expected {comb[i]};
        bool reset_on_path_occured {false};
        for (std::size_t j {1}; j <= errors.getLatency(); ++j){
            if (i + j < sample.size() && sample[i + j].reset == 1){
                expected.valid = 0;
                expected.y = 0;
                reset_on_path_occured = true;
                break;
            }
        }
        if (expected.y != pipe[i + errors.getLatency()].y || 
            expected.valid != pipe[i + errors.getLatency()].valid ){
            Error error;    
            error.input_cycle = i;
            error.pipe_cycle = i + errors.getLatency();
            error.real_valid = pipe[i + errors.getLatency()].valid;
            error.real_y = pipe[i + errors.getLatency()].y;
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
    error_finding(comb_outputs, pipe_outputs, sample, errors);
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
    error_finding(comb_outputs, pipe_outputs, sample, errors);
    print_both_models(comb_outputs, pipe_outputs, sample, 2);
    if (errors.size() == 0) std::cout << "\nCheck: OK\n\n";
    else{
        std::cout << "\nCheck: FAILED\n\n";
        if (errors.size() > 5) errors.print_first();
        else errors.print_all();
    }
}

int output_comparison(const std::vector<OutputSample>& in_file,
                       const std::vector<OutputSample>& out_file,
                       std::vector<ExternalComparisonRow>& rows){
    int count {};
    if (in_file.size() != out_file.size())
        throw std::invalid_argument("\nWrong sizes of input and output files\n");
    for (std::size_t i {}; i < in_file.size(); ++i){
        ExternalComparisonRow row;
        row.expected_valid = in_file[i].valid;
        row.expected_y = in_file[i].y;
        row.actual_valid = out_file[i].valid;
        row.actual_y = out_file[i].y;
        row.cycle = i;
        row.result = "OK";
        if (in_file[i].y != out_file[i].y || 
            in_file[i].valid != out_file[i].valid){
            row.result = "FAIL";
            ++count;
        } 
        rows.push_back(row);
    }
    return count;
}

void print_input_output_table(const std::vector<ExternalComparisonRow>& rows){
    std::cout << '\n' << table_sep 
    << std::format("{:^{}}", "cycle", width_for_column) << '|'
    << std::format("{:^{}}", "expected valid", width_for_column) << '|'
    << std::format("{:^{}}", "expected y", width_for_column) << '|'
    << std::format("{:^{}}", "actual valid", width_for_column) << '|'
    << std::format("{:^{}}", "actual y", width_for_column) << '|'
    << std::format("{:^{}}", "result correct", width_for_column) << '|'
    << '\n' << table_sep;
    for (std::size_t i {}; i < rows.size(); ++i){
        std::string cycle {"cycle "};
        cycle += std::to_string(i);
        std::cout << std::format("{:^{}}", cycle, width_for_column) << '|'
        << std::format("{:^{}}", rows[i].expected_valid, width_for_column) << '|'
        << std::format("{:^{}}", rows[i].expected_y, width_for_column) << '|'
        << std::format("{:^{}}", rows[i].actual_valid, width_for_column) << '|'
        << std::format("{:^{}}", rows[i].actual_y, width_for_column) << '|'
        << std::format("{:^{}}", rows[i].result, width_for_column) << '|'
        << '\n' << table_sep;
    }
    std::cout << '\n';
}

void comparison_of_input_output_models(const std::string& input_file,
                                       const std::string& output_file, int latency){
    std::vector<InputSample> in_file {};
    std::vector<OutputSample> out_file {};
    in_file = read_input_trace(input_file);
    out_file = read_output_file(output_file);
    std::vector<OutputSample> pipes{};
    if (latency == 1){
        PipelinedModel pipe{};
        for (std::size_t i {}; i < in_file.size(); ++i){
            pipes.push_back(pipe.tick(in_file[i].reset, in_file[i].valid, 
                                      in_file[i].a, in_file[i].b, in_file[i].c));
        }
        pipes.push_back(pipe.tick(0, 0, 0, 0, 0));
    }
    else if (latency == 2){
        TwoStagePipelineModel pipe{};
        for (std::size_t i {}; i < in_file.size(); ++i){
            pipes.push_back(pipe.tick(in_file[i].reset, 
                                      in_file[i].valid, 
                                      in_file[i].a, 
                                      in_file[i].b, 
                                      in_file[i].c));
        }
        pipes.push_back(pipe.tick(0, 0, 0, 0, 0));
        pipes.push_back(pipe.tick(0, 0, 0, 0, 0));
    }
    else{
        throw std::invalid_argument("\nWrong latency parameter\n");
    }
    std::vector<ExternalComparisonRow> rows {};
    int count {output_comparison(pipes, out_file, rows)};
    print_input_output_table(rows);
    if (count == 0) std::cout << "\nCheck: OK\n\n";
    else{
        std::cout << "\nCheck: FAILED\n\n";
        for (std::size_t i {}; i < rows.size(); ++i){
            if (rows[i].result == "FAIL"){
                std::cout << "Missmatch at cycle - " <<
                std::to_string(rows[i].cycle) << "\tExpected valid - "
                << rows[i].expected_valid << "\tReal valid - "
                << rows[i].actual_valid << "\t\tExpected y - "
                << rows[i].expected_y << "\t\tReal y - "
                << rows[i].actual_y << "\n\n";
            }
        }
    }
}