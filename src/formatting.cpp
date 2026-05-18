#include "formatting.h"

std::string full_line(){
    std::string line {};
    for (int i {}; i < columns_amount; ++i){
        std::string part_of_line(width, '-');
        line += part_of_line + '+';
    }
    line += '\n';
    return line;
}

std::string table_line(){
    std::string line {};
    for (int i {}; i < columns_in_table; ++i){
        std::string part_of_line(width_for_column, '-');
        line += part_of_line + '+';
    }
    line += '\n';
    return line;
}