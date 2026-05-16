#include "formatting.h"

std::string line(){
    std::string line {};
    for (int i {}; i < columns_amount; ++i){
        std::string part_of_line(width, '-');
        line += part_of_line + '+';
    }
    line += '\n';
    return line;
}
