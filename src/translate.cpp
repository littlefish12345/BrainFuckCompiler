#include <translate.hpp>
#include <platform_linux_amd64.hpp>

#include <string>
#include <fstream>

void translate(std::string source, long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out, target_platform target) {
    switch (target) {
        case platform_linux_amd64:
            linux_amd64::translate(source, memory_size, stack_size, jump_entry_num, out);
            break;
    
        default:
            break;
    }
}