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
