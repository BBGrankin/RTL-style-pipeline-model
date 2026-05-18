#pragma once

#include <vector>

struct Error{
    int input_cycle;
    int pipe_cycle;
    int expected_valid;
    int expected_y;
    int real_valid;
    int real_y;
    bool reset;
};

class ErrorsLog{
    private:
        std::vector<Error> errors;
        std::size_t latency;

    public:

        void setLatency(std::size_t latency){
            this->latency = latency;
        }

        std::size_t getLatency() const{
            return latency;
        }

        std::size_t size() const;

        void push_back(const Error &error);

        void print_all() const;

        void print_first() const;
};