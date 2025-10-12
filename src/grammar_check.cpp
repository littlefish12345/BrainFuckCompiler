#include <grammar_check.hpp>

#include <cstdio>
#include <string>

int grammar_check(std::string source) {
    unsigned long long line_counter = 1;
    unsigned long long colum_counter = 0;

    long long depth = 0;
    long long jump_mark_count = 0;
    for (long long i = 0; i < source.length(); ++i) {
        if (source[i] == '\n') {
            line_counter += 1;
            colum_counter = 0;
        } else {
            ++colum_counter;
        }
        switch (source[i]) {
            case '[':
                ++depth;
                break;
            case ']':
                --depth;
                if (depth < 0) {
                    printf("Error: Unclosed loop mark at line %lld colum %lld\n", line_counter, colum_counter);
                    exit(1);
                }
                break;
            case '$':
                ++jump_mark_count;
                if (jump_mark_count > 256) {
                    printf("Warning: Unreachable jump mark at line %lld colum %lld\n");
                }
                break;
        
            default:
                break;
        }
    }

    if (depth != 0) {
        printf("Error: Unclosed loop mark in the end\n");
        exit(1);
    }
    return jump_mark_count;
}
