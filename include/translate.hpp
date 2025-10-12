#pragma once

#include <string>
#include <fstream>

enum target_platform {
    platform_linux_amd64,
};

/*
operator to num
+ 0
- 1
< 2
> 3
[ 4
] 5
, 6
. 7
*/

void translate(std::string source, long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out, target_platform target);