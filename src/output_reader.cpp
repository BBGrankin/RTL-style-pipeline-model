#include "output_reader.h"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include "trace_reader.h"

std::vector<OutputSample> read_output_file(const std::string& file_name){
    std::ifstream file {file_name};
    if (!file){
        throw std::invalid_argument("\nCouldn`t read output file\n");
    }
    std::vector<OutputSample> output_vector {};
    std::string line {};
    int line_count {};
    while (std::getline(file, line)){
        ++line_count;
        if (line.empty()) continue;
        std::istringstream iss {line};
        std::vector<std::string> container {};
        std::string word {};
        while (iss >> word){
            container.push_back(word);
        }
        if (container.size() != 2) 
            throw std::invalid_argument("\nWrong number of "
                                        "parameters in output file on line " +
                                        std::to_string(line_count) + '\n');
        
        for (std::size_t i {}; i < container.size(); ++i){
            std::string message {};
            if (!is_token_number(container[i])){ 
                message = "\nNot a number in output file on line " 
                          + std::to_string(line_count) + " on position " 
                          + std::to_string(i + 1) + '\n';
                throw std::invalid_argument(message);
            }
        }
        std::size_t position {};
        std::vector<int> values {};
        try {
            for (; position < container.size(); ++position){
                values.push_back(std::stoi(container[position]));
            }
        }
        catch(const std::exception&) {
            throw std::invalid_argument("\nCouldn`t convert string to "
                                        "int in output file on line " + 
                                        std::to_string(line_count) + 
                                        " on position " + 
                                        std::to_string(position + 1) + '\n');
        }
        if (values[0] != 1 && values[0] != 0)
            throw std::invalid_argument("Wrong valid argument in output "
                                        "file on line " + 
                                        std::to_string(line_count));
        OutputSample output_line {values[0], values[1]};
        output_vector.push_back(output_line);
    }
    if (output_vector.size() == 0)
        throw std::invalid_argument("\nOutput file is empty\n");
    return output_vector;
}