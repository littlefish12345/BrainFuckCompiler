#pragma once

#include <string>
#include <fstream>

namespace linux_amd64 {
    void translate(std::string source, long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out);
};