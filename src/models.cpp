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


TwoStagePipelineModel::TwoStagePipelineModel(int stage1_sum, int stage1_c, 
                                             int stage1_valid, int stage2_sum,
                                             int stage2_c, int stage2_valid):
    stage1_sum(stage1_sum), stage1_c(stage1_c), stage1_valid(stage1_valid),
    stage2_sum(stage2_sum), stage2_c(stage2_c), stage2_valid(stage2_valid)
{}

OutputSample TwoStagePipelineModel::tick(int reset, int valid, 
                                         int a, int b, int c){
    OutputSample output {};
    if (reset == 1){
        stage1_c = 0;
        stage1_sum = 0;
        stage1_valid = 0;

        stage2_c = 0;
        stage2_sum = 0;
        stage2_valid = 0;

        output.valid = 0;
        output.y = 0;
        return output;
    }

    output.valid = stage2_valid;
    if (stage2_valid == 0){
        output.y = 0;
    }
    else {
        output.y = stage2_c * stage2_sum;
    }
    stage2_valid = stage1_valid;
    stage2_c = stage1_c;
    stage2_sum = stage1_sum;

    if (valid == 0){
        stage1_c = 0;
        stage1_sum = 0;
        stage1_valid = 0;
    }
    else {
        stage1_c = c;
        stage1_sum = a + b;
        stage1_valid = valid;
    }
    return output;
}

RetimedTwoStagePipelineModel::RetimedTwoStagePipelineModel(int stage1_a, 
                                                           int stage1_b,
                                                           int stage1_c, 
                                                           int stage1_valid,
                                                           int stage2_valid, 
                                                           int stage2_y):
    stage1_a(stage1_a), stage1_b(stage1_b), stage1_c(stage1_c), 
    stage1_valid(stage1_valid), stage2_valid(stage2_valid), stage2_y(stage2_y)
{}

OutputSample RetimedTwoStagePipelineModel::tick(int reset, int valid, int a, int b, int c){
    OutputSample output;
    if (reset == 1){
        output.valid = 0;
        output.y = 0;

        stage1_a = 0;
        stage1_b = 0;
        stage1_c = 0;
        stage1_valid = 0;
        stage2_valid = 0;
        stage2_y = 0;
        return output;
    }

    output.valid = stage2_valid;
    output.y = stage2_y;

    if (stage1_valid == 1){
        stage2_y = (stage1_b + stage1_a) * stage1_c;
        stage2_valid = stage1_valid;
    }
    else{
        stage2_y = 0;
        stage2_valid = 0;
    }
    
    if (valid == 1){
        stage1_a = a;
        stage1_b = b;
        stage1_c = c;
        stage1_valid = valid;
    }
    else{
        stage1_a = 0;
        stage1_b = 0;
        stage1_c = 0;
        stage1_valid = 0;
    }
    return output;
}