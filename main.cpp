#include <vector>
#include <iostream>
#include <string_view>

const std::string_view separator {"  |  "};

const std::string_view line {"-----------------------------------------------------------------------\n"};

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
    std::cout << "cycle " << separator << "reset"
    << separator << "valid" << separator << 'a'
    << separator << 'b' << separator << 'c' << separator
    << " combs " << separator << " pipes " << separator << '\n' << line;
    for (std::size_t i {}; i < comb.size(); ++i){
        std::cout << "cycle" << i << separator << "  " << sample[i].reset
        << "  " << separator << "  " << sample[i].valid
        << "  " << separator << sample[i].a << separator
        << sample[i].b << separator << sample[i].c 
        << separator << comb[i].valid << separator << comb[i].y 
        << separator << pipe[i].valid << separator << pipe[i].y 
        << separator << '\n' << line;
    }
    std::cout << "cycle" << pipe.size() - 1 << separator << "  0  " 
    << separator << "  0  " << separator << 0 
    << separator << 0 << separator << 0
    << separator << 0 << separator << 0 << separator << pipe.back().valid
    << separator << pipe.back().y << separator << '\n'; 
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
    bool is_equal {true};
    for (std::size_t i {}; i < comb_outputs.size(); ++i){
        OutputSample expected {};
        if (i + 1 < sample.size() && sample[i + 1].reset == 1){
            expected.valid = 0;
            expected.y = 0;
        }
        else{
            expected = comb_outputs[i];
        }
        if (expected.y != pipe_outputs[i + 1].y || 
            expected.valid != pipe_outputs[i + 1].valid ){
            is_equal = false;
            break;
        } 
    }
    print_both_models(comb_outputs, pipe_outputs, sample);
    if (is_equal){
        std::cout << "\nCheck result: OK\n";
    }
    else{
        std::cout << "Check result: FAILED\n";
    }
}

int main(){
    std::vector<InputSample> sample {
    {1, 0, 0, 0, 0},
    {0, 1, 1, 2, 3},
    {0, 1, 4, 5, 6},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 3, 2},
    {0, 1, 2, 2, 5}
    };
    model_comparison(sample);
    return 0;
}