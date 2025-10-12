#pragma once

#include <translate.hpp>

#include <string>

void compile(std::string source_path, std::string out_path, target_platform platform, int memory_size, int stack_size, bool out_asm, bool out_obj);