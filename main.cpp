#include <vector>
#include <iostream>
#include <string_view>

const std::string_view separator {"  |  "};

const std::string_view line {"------------------------------------------------------"
    "----------------------------------------------\n"};

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
    public:
        std::size_t size() const{
            return errors.size();
        }

        void push_back(const Error &error){
            errors.push_back(error);
        }

        void print_all() const {
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
                                 "from combinational output with latency 1\n"
                    << line << '\n';
            }
        }

        void print_first() const{
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
                                "from combinational output with latency 1\n"
                << line << '\n';     
        }
};

struct InputSample{
    int reset;
    int valid;
    int a;
    int b;
    int c;
};

struct OutputSample{
    int valid;
    int y;
};

class PipelinedModel{
    private:
        int sum_reg;
        int c_reg;
        int valid_reg;

    public:   
        PipelinedModel(int sum_reg = 0, int c_reg = 0, int valid_reg = 0): 
                       sum_reg(sum_reg), c_reg(c_reg), valid_reg(valid_reg)
        {}

        OutputSample tick (int reset, int valid, int a, int b, int c){
            OutputSample output {};
            if (reset == 1){
                output.valid = 0;
                output.y = 0;
                sum_reg = 0;
                c_reg = 0;
                valid_reg = 0;
                return output;
            }
            output.valid = valid_reg;
            if (valid_reg == 0) output.y = 0;
            else output.y = sum_reg * c_reg;
            sum_reg = a + b;
            c_reg = c;
            valid_reg = valid;
            return output;
        }
};

class CombinationModel{
    public:
        OutputSample tick(int reset, int valid, int a, int b, int c){
            OutputSample output;
            if (reset == 1){
                output.valid = 0;
                output.y = 0;
                return output;
            }
            if (valid == 0){
                output.valid = 0;
                output.y = 0;
            }
            else{
                output.valid = valid;
                output.y = (a + b) * c;
            }
            return output;
        }
};

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

int main(){
    std::vector<InputSample> sample {
    {1, 0, 0, 0, 0},
    {0, 1, 1, 2, 1},
    {0, 1, 4, 5, 1},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 3, 1},
    {0, 1, 2, 2, 1}
    };
    model_comparison(sample);
    return 0;
}