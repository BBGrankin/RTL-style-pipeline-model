#include "trace_reader.h"
#include <fstream>
#include <stdexcept>
#include <sstream>

std::vector<InputSample> read_input_trace (const std::string& file_name){
    std::ifstream file {file_name};
    std::vector<InputSample> samples {};
    InputSample sample;
    int line_count {};
    std::string line {};
    std::string message {};
    if (!file){
        throw std::invalid_argument("\nCouldnt`t read file\n");
    }
    while (std::getline(file, line)){
        ++line_count;
        if (line.empty()) continue;
        std::istringstream iss {line};
        std::vector<std::string> container{};
        std::string word{};
        while (iss >> word){
            container.push_back(word);
        }
        if (container.size() != 5){
            message = "\nWrong number of parameters"
                        " in line " + std::to_string(line_count) + '\n';
            throw std::invalid_argument(message);
        }
        try {
            sample.reset = std::stoi(container[0]);
            sample.valid = std::stoi(container[1]);
            sample.a = std::stoi(container[2]);
            sample.b = std::stoi(container[3]);
            sample.c = std::stoi(container[4]);
        }
        catch(const std::exception& e) {
            message = "\nNot a number in line " 
                        + std::to_string(line_count) + '\n';
            throw std::invalid_argument(message);
        }
        if (sample.reset > 1 || sample.reset < 0
            || sample.valid < 0 || sample.valid > 1){
            message = "\nWrong valid / reset argument in line " 
                      + std::to_string(line_count) + '\n';
            throw std::invalid_argument(message);
        }
        samples.push_back(sample);
    }
    if (samples.size() == 0)
        throw std::invalid_argument("\nFile is empty\n");
    return samples;
}
