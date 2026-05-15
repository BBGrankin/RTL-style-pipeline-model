#pragma once

#include "samples.h"

class PipelinedModel{
    private:
        int sum_reg;
        int c_reg;
        int valid_reg;

    public:   
        PipelinedModel(int sum_reg = 0, int c_reg = 0, int valid_reg = 0);

        OutputSample tick (int reset, int valid, int a, int b, int c);
};

class CombinationModel{
    public:
        OutputSample tick(int reset, int valid, int a, int b, int c);
};

class TwoStagePipelineModel {
    private:
        int stage1_sum;
        int stage1_c;
        int stage1_valid;

        int stage2_sum;
        int stage2_c;
        int stage2_valid;

    public:
        TwoStagePipelineModel(int stage1_sum = 0, int stage1_c = 0, 
                              int stage1_valid = 0, int stage2_sum = 0,
                              int stage2_c = 0, int stage2_valid = 0);
        OutputSample tick(int reset, int valid, int a, int b, int c);

};