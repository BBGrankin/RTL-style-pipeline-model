#pragma once

#include <vector>
#include "samples.h"
#include <string>

std::vector<InputSample> read_input_trace (const std::string& file_name);

bool is_token_number (const std::string& token);