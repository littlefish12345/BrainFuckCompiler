#include <platform_linux_amd64.hpp>

#include <string>
#include <fstream>
#include <vector>

namespace linux_amd64 {
    void initialize_translate_context(long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out) {
        out << "%define MEMORY_SIZE "<< memory_size << "\n%define STACK_SIZE " << stack_size << "\n";
        out << "%define STACK_TOP stack+32*8\n%define JUMP_ENTRY_NUM " << jump_entry_num << "\n\n";
        out << "section .bss\n\tmemory resb " << memory_size << "\n\tstack resq " << stack_size << "\n\t";
        out << "term_cfg_org resb 36\n\tterm_cfg_new resb 36\n\n";
        out << "section .data\n\t";
        out << "stack_overflow_msg db 'Stack Overflow', 0x0A\n\tstack_overflow_msg_len equ $ - stack_overflow_msg\n\t";
        out << "jump_error_msg db 'Jump Error', 0x0A\n\tjump_error_msg_len equ $ - jump_error_msg\n\t";
        out << "ret_error_msg db 'Return Error', 0x0A\n\tret_error_msg_len equ $ - ret_error_msg\n";
        out << "jump_table:\n";
        for (int i = 0; i < jump_entry_num; ++i) {
            out << "\tdq jump_entry_" << i << "\n";
        }
        out << "\nsection .text\n";
        out << "stack_overflow:\n\tlea rsi, [stack_overflow_msg]\n\tmov rdx, stack_overflow_msg_len\n\tjmp common_error_exit\n\n";
        out << "jump_error:\n\tlea rsi, [jump_error_msg]\n\tmov rdx, jump_error_msg_len\n\tjmp common_error_exit\n\n";
        out << "ret_error:\n\tlea rsi, [ret_error_msg]\n\tmov rdx, ret_error_msg_len\n\n";
        out << "common_error_exit:\n\tmov eax, 1\n\tmov edi, 1\n\tsyscall\n\n\t";
        out << "mov eax, 16\n\txor rdi, rdi\n\tmov rsi, 0x5402\n\tlea rdx, [term_cfg_org]\n\tsyscall\n\n\t";
        out << "mov eax, 60\n\tmov edi, 1\n\tsyscall\n\n";
        out << "global _start\n_start:\n\t";
        out << "mov eax, 16\n\txor rdi, rdi\n\tmov rsi, 0x5401\n\tlea rdx, [term_cfg_org]\n\tsyscall\n\t";
        out << "mov eax, 16\n\txor rdi, rdi\n\tmov rsi, 0x5401\n\tlea rdx, [term_cfg_new]\n\tsyscall\n\t";
        out << "mov dword [term_cfg_new+12], 0\n\tmov byte [term_cfg_new+18], 1\n\tmov byte [term_cfg_new+19], 0\n\t";
        out << "mov eax, 16\n\txor rdi, rdi\n\tmov rsi, 0x5402\n\tlea rdx, [term_cfg_new]\n\tsyscall\n\t";
        out << "mov rsp, STACK_TOP\n\txor r8, r8\n\tmov r9, " << memory_size << "\n\t";
        out << "mov r10, " << memory_size-1 << "\n\tmov r11, 0\n";
    }

    void exit_translate_context(std::ofstream &out) {
        out << "\tmov eax, 16\n\txor rdi, rdi\n\tmov rsi, 0x5402\n\tlea rdx, [term_cfg_org]\n\tsyscall\n";
        out << "\tmov eax, 60\n\txor rdi, rdi\n\tsyscall\n";
    }

    void translate(std::string source, long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out) {
        initialize_translate_context(memory_size, stack_size, jump_entry_num, out);

        std::vector<long long> loop_depth_counter = std::vector<long long>();
        long long depth = 0;
        int jump_entry_count = 0;
        
        for (long long i = 0; i < source.length(); ++i) {
            switch (source[i]) {
                case '+': //standard set
                    out << "\tinc byte [memory+r8]\n";
                    break;
                case '-':
                    out << "\tdec byte [memory+r8]\n";
                    break;
                case '<':
                    out << "\tdec r8\n\tcmp r8, 0\n\tcmovl r8, r10\n";
                    break;
                case '>':
                    out << "\tinc r8\n\tcmp r8, MEMORY_SIZE-1\n\tcmovae r8, r11\n";
                    break;
                case '[':
                    ++depth;
                    if (loop_depth_counter.size() < depth) {
                        loop_depth_counter.push_back(0);
                    }
                    ++loop_depth_counter[depth-1];
                    out << "loop_start_depth_"<< depth << "_no_" << loop_depth_counter[depth-1] << ":\n";
                    out << "\tcmp byte [memory+r8], 0\n";
                    out << "\tje " << "loop_end_depth_" << depth << "_no_" << loop_depth_counter[depth-1] << "\n";
                    break;
                case ']':
                    out << "\tjmp " << "loop_start_depth_"<< depth << "_no_" << loop_depth_counter[depth-1] <<"\n";
                    out << "loop_end_depth_" << depth << "_no_" << loop_depth_counter[depth-1] << ":\n";
                    --depth;
                    break;
                case ',':
                    out << "\txor rax, rax\n\txor rdi, rdi\n\tlea rsi, [memory+r8]\n\tmov edx, 1\n\tsyscall\n";
                    //break; //output once default
                case '.':
                    out << "\tmov eax, 1\n\tmov edi, 1\n\tlea rsi, [memory+r8]\n\tmov edx, 1\n\tsyscall\n";
                    break;
                case '(': //extend set
                    out << "\tshl byte [memory+r8], 1\n";
                    break;
                case ')':
                    out << "\tshr byte [memory+r8], 1\n";
                    break;
                case '@':
                    out << "\tmov byte [memory+r8], 0\n";
                    break;
                case '^':
                    out << "\tmovzx rax, byte [memory+r8]\n\txor rdx, rdx\n\tdiv r9\n\tmov r8, rdx\n";
                    break;
                case '|':
                    out << "\txor r8, r8\n";
                    break;
                case '!':
                    out << "\tlea rdi, [memory]\n\txor al, al\n\tmov rcx, r9\n\tcld\n\trep stosb\n";
                    break;
                case '$':
                    out << "jump_entry_" << jump_entry_count << ":\n";
                    ++jump_entry_count;
                    break;
                case '*':
                    out << "\tmovzx rax, byte [memory+r8]\n\tcmp rax, JUMP_ENTRY_NUM\n\tjae jump_error\n";
                    out << "\tmov rax, [jump_table+rax*8]\n\tjmp rax\n";
                    break;
                case '{':
                    out << "\tcmp rsp, stack\n\tjbe stack_overflow\n";
                    out << "\tmovzx rax, byte [memory+r8]\n\tcmp rax, JUMP_ENTRY_NUM\n\tjae jump_error\n";
                    out << "\tmov rax, [jump_table+rax*8]\n\tcall rax\n";
                    break;
                case '}':
                    out << "\tcmp rsp, STACK_TOP\n\tjae ret_error\n\tret\n";
                    break;
                case '%':
                    out << "\tmovzx rax, byte [memory+r8]\n\tmovzx rax, byte [memory+rax]\n\tmov byte [memory+r8], al\n";
                    break;

                default:
                    break;
            }
        }

        exit_translate_context(out);
    }
};