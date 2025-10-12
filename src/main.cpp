#include <compile.hpp>

#include <cstdio>
#include <cstring>
#include <string>

long long memory_size = 256;
long long stack_size = 32;
std::string source_path = "";
std::string out_path = "a.out";
target_platform platform = platform_linux_amd64;

const char * help_text = R"(Usage: bfcc [Options] File
    Options:
        -h            Print help text.
        -a            Output assembly file
        -c            Output to object file
        -sm           Specify memory size (default 256).
        -ss           Specify stack size (default 32).
        -o <Filename> Output to <Filename>.
)";

int main(int argc, char *argv[]) {
    int i = 1;
    bool out_asm = false;
    bool out_obj = false;
    int len;
    while (i < argc) {
        len = strlen(argv[i]);
        if (len == 0) {
            ++i;
            continue;
        }
        if (argv[i][0] == '-') {
            if (len == 1) {
                printf("Unknown option -. Use -h to print help text.\n");
                return 1;
            }
            switch (argv[i][1]) {
                case 'h':
                    printf(help_text);
                    return 0;
                    break;
                case 'a':
                    out_asm = true;
                    out_obj = false;
                    break;
                case 'c':
                    out_asm = false;
                    out_obj = true;
                    break;
                case 'o':
                    ++i;
                    if (i >= argc) {
                        printf("Not enough argument after -o. Use -h to print help text.\n");
                        return 1;
                    }
                    out_path = argv[i];
                    break;
                case 's':
                    if (len == 2) {
                        printf("Unknown option -s. Use -h to print help text.\n");
                        return 1;
                    }
                    switch (argv[i][2]) {
                        case 'm':
                            ++i;
                            if (i >= argc) {
                                printf("Not enough argument after -sm. Use -h to print help text.\n");
                                return 1;
                            }
                            sscanf(argv[i], "%lld", &memory_size);
                            if (memory_size <= 0) {
                                printf("Cannot set memory size smaller or equal to 0.\n");
                                return 1;
                            }
                            break;
                        case 's':
                            ++i;
                            if (i >= argc) {
                                printf("Not enough argument after -ss. Use -h to print help text.\n");
                                return 1;
                            }
                            sscanf(argv[i], "%lld", &stack_size);
                            if (stack_size < 0) {
                                printf("Cannot set stack size smaller than 0.\n");
                                return 1;
                            }
                            break;

                        default:
                            printf("Unknown option -s%c. Use -h to print help text.\n", argv[i][2]);
                            return 1;
                    }
                    break;

                default:
                    printf("Unknown option -%c. Use -h to print help text.\n", argv[i][1]);
                    return 1;
            }
            ++i;
        } else {
            source_path = argv[i];
            ++i;
        }
    }

    if (source_path == "") {
        printf("No input file. Use -h to print help text.\n");
        return 1;
    }

    compile(source_path, out_path, platform, memory_size, stack_size, out_asm, out_obj);

    return 0;
}