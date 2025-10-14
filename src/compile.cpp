#include <compile.hpp>
#include <grammar_check.hpp>
#include <strip_source.hpp>
#include <translate.hpp>

#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>

void compile(std::string source_path, std::string out_path, target_platform platform, int memory_size, int stack_size, bool out_asm, bool out_obj) {
    std::ifstream source_file = std::ifstream(source_path);
    if (!source_file.is_open()) {
        printf("Can't open file %s\n", source_path.c_str());
        exit(1);
    }
    std::ostringstream ss;
    ss << source_file.rdbuf();
    std::string source = ss.str();

    source = strip_source(source);
    printf("%s", source.c_str());
    int jump_entry_count = grammar_check(source);

    std::string asm_out_path = out_asm ? out_path : "out.asm";

    std::ofstream out_file = std::ofstream(asm_out_path, std::ios::out);
    translate(source, memory_size, stack_size, jump_entry_count, out_file, platform);
    out_file.close();

    if (out_asm) {
        return;
    }

    std::string exe_type = "";

    switch (platform) {
        case platform_linux_amd64:
            exe_type = "elf64";
            break;
    
        default:
            break;
    }

    std::string obj_out_path = out_obj ? out_path : "out.o";

    FILE* nasm_pipe = popen(std::string("nasm -f " + exe_type + " out.asm -o " + obj_out_path + " 2>&1").c_str(), "r");
    if (nasm_pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), nasm_pipe) != nullptr) {
            printf("%s", buffer);
        }
        pclose(nasm_pipe);
    }
    remove ("out.asm");

    if (out_obj) {
        return;
    }

    FILE* ld_pipe = popen(std::string("ld out.o -o " + out_path + " 2>&1").c_str(), "r");
    if (ld_pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), ld_pipe) != nullptr) {
            printf("%s", buffer);
        }
        pclose(ld_pipe);
    }
    remove("out.o");
}