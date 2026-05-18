#pragma once

#include <string>

const int width {12};

const int columns_amount {10};

std::string full_line();

const std::string line {full_line()};

const int width_for_column {16};

const int columns_in_table {6};

std::string table_line();

const std::string table_sep {table_line()};