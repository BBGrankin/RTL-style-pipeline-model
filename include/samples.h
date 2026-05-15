#pragma once

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