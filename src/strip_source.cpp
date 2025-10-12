#include <strip_source.hpp>

#include <string>
#include <sstream>

std::string strip_source(std::string source) {
    std::stringstream output;
    for (long long i = 0; i < source.length(); ++i) {
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
                break;
        
            default:
                break;
        }
    }
    return output.str();
}