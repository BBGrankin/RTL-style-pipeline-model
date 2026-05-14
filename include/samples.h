#pragma once

#include <string_view>

const std::string_view separator {"  |  "};

const std::string_view line {"------------------------------------------------------"
    "----------------------------------------------\n"};

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