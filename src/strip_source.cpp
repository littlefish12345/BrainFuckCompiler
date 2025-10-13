#include <strip_source.hpp>

#include <string>
#include <sstream>

std::string strip_source(std::string source) {
    std::stringstream output;
    long long i = 0;
    while (i < source.length()) {
        switch (source[i]) {
            case '+': //standard set
            case '-':
            case '<':
            case '>':
            case '[':
            case ']':
            case ',':
            case '.':
            case '(': //extend set
            case ')':
            case '@':
            case '^':
            case '|':
            case '!':
            case '$':
            case '*':
            case '{':
            case '}':
            case '%':
                output << source[i];
                ++i;
                break;

            case '#':
                while (i < source.length() && source[i] != '\n') {
                    ++i;
                }
                ++i;
                break;
        
            default:
                ++i;
                break;
        }
    }
    return output.str();
}