#include <platform_linux_amd64.hpp>

#include <string>
#include <fstream>
#include <vector>

namespace linux_amd64 {
    void initialize_translate_context(long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out) {
        out << "section .bss\n\tmemory resb " << memory_size << "\n\tstack resq " << stack_size << "\n\t";
        out << "term_cfg_org resb 36\n\tterm_cfg_new resb 36\n\n";
        out << "section .data\n\tmemory_size dq " << memory_size << "\n\tstack_size dq " << stack_size << "\n\t";
        out << "pointer dq 0\n\tstack_top dq stack+" << stack_size << "*8\n\tjump_entry_num dq " << jump_entry_num << "\n\t";
        out << "stack_overflow_msg db 'Stack Overflow', 0x0A\n\tstack_overflow_msg_len equ $ - stack_overflow_msg\n\t";
        out << "jump_error_msg db 'Jump Error', 0x0A\n\tjump_error_msg_len equ $ - jump_error_msg\n\t";
        out << "ret_error_msg db 'Return Error', 0x0A\n\tret_error_msg_len equ $ - ret_error_msg\n\n";
        out << "section .text\njump_table:\n";
        for (int i = 0; i < jump_entry_num; ++i) {
            out << "\tdq jump_entry_" << i << "\n";
        }
        out << "\nstack_overflow:\n\tmov rax, 1\n\tmov rdi, 1\n\t";
        out << "mov rsi, stack_overflow_msg\n\tmov rdx, stack_overflow_msg_len\n\tsyscall\n\n\t";
        out << "mov rax, 60\n\tmov rdi, 1\n\tsyscall\n\n";
        out << "jump_error:\n\tmov rax, 1\n\tmov rdi, 1\n\t";
        out << "mov rsi, jump_error_msg\n\tmov rdx, jump_error_msg_len\n\tsyscall\n\n\t";
        out << "mov rax, 60\n\tmov rdi, 1\n\tsyscall\n\n";
        out << "ret_error:\n\tmov rax, 1\n\tmov rdi, 1\n\t";
        out << "mov rsi, ret_error_msg\n\tmov rdx, ret_error_msg_len\n\tsyscall\n\n\t";
        out << "mov rax, 60\n\tmov rdi, 1\n\tsyscall\n\n";
        out << "global _start\n_start:\n\t";
        out << "mov rax, 16\n\tmov rdi, 0\n\tmov rsi, 0x5401\n\tmov rdx, term_cfg_org\n\tsyscall\n\t";
        out << "mov rax, 16\n\tmov rdi, 0\n\tmov rsi, 0x5401\n\tmov rdx, term_cfg_new\n\tsyscall\n\t";
        out << "mov dword [term_cfg_new+12], 0\n\tmov byte [term_cfg_new+18], 1\n\tmov byte [term_cfg_new+19], 0\n\t";
        out << "mov rax, 16\n\tmov rdi, 0\n\tmov rsi, 0x5402\n\tmov rdx, term_cfg_new\n\tsyscall\n\t";
        out << "mov rsp, [stack_top]\n";
    }

    void exit_translate_context(std::ofstream &out) {
        out << "\tmov rax, 16\n\tmov rdi, 0\n\tmov rsi, 0x5402\n\tmov rdx, term_cfg_org\n\tsyscall\n";
        out << "\tmov rax, 60\n\tmov rdi, 0\n\tsyscall\n";
    }

    void translate(std::string source, long long memory_size, long long stack_size, int jump_entry_num, std::ofstream &out) {
        initialize_translate_context(memory_size, stack_size, jump_entry_num, out);

        std::vector<long long> loop_depth_counter = std::vector<long long>();
        long long depth = 0;
        int jump_entry_count = 0;
        
        for (long long i = 0; i < source.length(); ++i) {
            switch (source[i]) {
                case '+': //standard set
                    out << "\tmov rsi, [pointer]\n\tinc byte [memory+rsi]\n";
                    break;
                case '-':
                    out << "\tmov rsi, [pointer]\n\tdec byte [memory+rsi]\n";
                    break;
                case '<':
                    out << "\tdec qword [pointer]\n";
                    out << "\tmov rax, [pointer]\n\tmov rdx, 0\n\tdiv qword [memory_size]\n\tmov [pointer], rdx\n";
                    break;
                case '>':
                    out << "\tinc qword [pointer]\n";
                    out << "\tmov rax, [pointer]\n\tmov rdx, 0\n\tdiv qword [memory_size]\n\tmov [pointer], rdx\n";
                    break;
                case '[':
                    ++depth;
                    if (loop_depth_counter.size() < depth) {
                        loop_depth_counter.push_back(0);
                    }
                    ++loop_depth_counter[depth-1];
                    out << "loop_start_depth_"<< depth << "_no_" << loop_depth_counter[depth-1] << ":\n";
                    out << "\tmov rsi, [pointer]\n\tcmp byte [memory+rsi], 0\n";
                    out << "\tje " << "loop_end_depth_" << depth << "_no_" << loop_depth_counter[depth-1] << "\n";
                    break;
                case ']':
                    out << "\tjmp " << "loop_start_depth_"<< depth << "_no_" << loop_depth_counter[depth-1] <<"\n";
                    out << "loop_end_depth_" << depth << "_no_" << loop_depth_counter[depth-1] << ":\n";
                    --depth;
                    break;
                case ',':
                    out << "\tmov rax, 0\n\tmov rdi, 0\n\tmov rsi, memory\n\tadd rsi, [pointer]\n\tmov rdx, 1\n\tsyscall\n";
                    //break; //output once default
                case '.':
                    out << "\tmov rax, 1\n\tmov rdi, 1\n\tmov rsi, memory\n\tadd rsi, [pointer]\n\tmov rdx, 1\n\tsyscall\n";
                    break;
                case '(': //extend set
                    out << "\tmov rsi, [pointer]\n\tshl byte [memory+rsi], 1\n";
                    break;
                case ')':
                    out << "\tmov rsi, [pointer]\n\tshr byte [memory+rsi], 1\n";
                    break;
                case '@':
                    out << "\tmov rsi, [pointer]\n\tmov byte [memory+rsi], 0\n";
                    break;
                case '^':
                    out << "\tmov rsi, [pointer]\n\tmovzx rax, byte [memory+rsi]\n\tdiv qword [memory_size]\n\tmov [pointer], rdx\n";
                    break;
                case '|':
                    out << "\tmov [pointer], 0\n";
                    break;
                case '!':
                    out << "\tmov rdi, memory\n\tmov rax, 0\n\tmov rcx, [memory_size]\n\tcld\n\trep stosb\n";
                    break;
                case '$':
                    out << "jump_entry_" << jump_entry_count << ":\n";
                    ++jump_entry_count;
                    break;
                case '*':
                    out << "\tmov rsi, [pointer]\n\tmovzx rax, byte [memory+rsi]\n\tcmp rax, [jump_entry_num]\n\tjae jump_error\n";
                    out << "\tmov rsi, rax\n\tmov rax, jump_table\n\tmov rax, [rax+rsi*8]\n\tjmp rax\n";
                    break;
                case '{':
                    out << "\tcmp rsp, stack\n\tjbe stack_overflow\n";
                    out << "\tmov rsi, [pointer]\n\tmovzx rax, byte [memory+rsi]\n\tcmp rax, [jump_entry_num]\n\tjae jump_error\n";
                    out << "\tmov rsi, rax\n\tmov rax, jump_table\n\tmov rax, [rax+rsi*8]\n\tcall rax\n";
                    break;
                case '}':
                    out << "\tcmp rsp, [stack_top]\n\tjae ret_error\n\tret\n";
                    break;
                case '%':
                    out << "\tmov rsi, [pointer]\n\tmovzx rax, byte [memory+rsi]\n\tmov rsi, rax\n\tmovzx rax, byte [memory+rsi]\n\tmov rsi, [pointer]\n\tmov byte [memory+rsi], al\n";
                    break;

                default:
                    break;
            }
        }

        exit_translate_context(out);
    }
};