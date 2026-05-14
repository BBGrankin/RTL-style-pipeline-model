#include "models.h"

PipelinedModel::PipelinedModel(int sum_reg, int c_reg, int valid_reg): 
                sum_reg(sum_reg), c_reg(c_reg), valid_reg(valid_reg)
{}

OutputSample PipelinedModel::tick (int reset, int valid, int a, int b, int c){
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


OutputSample CombinationModel::tick(int reset, int valid, int a, int b, int c){
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
