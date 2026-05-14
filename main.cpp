#include <vector>
#include "models.h"
#include "samples.h"
#include "checker.h"

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