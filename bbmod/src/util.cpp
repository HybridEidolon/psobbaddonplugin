#include "util.h"

#include <fstream>
#include <memory>
#include <cstdarg>
#include <algorithm>

std::string string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::string str;
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while (1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

std::string bbmod_get_config_property(std::ifstream &infile, const std::string propname) {
    std::string line;
    std::string result = std::string("");

    /* TODO: Proper config parsing...  */
    infile.clear();
    infile.seekg(0, infile._Seekbeg);
    while (false == infile.eof()) {
        std::getline(infile, line);
        if (line.length()) {
            const char *p_line = line.c_str();
            const char *p_equal_char;

            p_equal_char = strchr(p_line, '=');
            if (p_equal_char) {
                int property_length = p_equal_char - p_line;
                if (property_length == propname.length() && !_strnicmp(p_line, propname.c_str(), property_length)) {
                    /* p_equal_char points to '=', there is at least one null byte after.
                     * Worst case is "someprop=" will make result the empty string..
                     */
                    result = std::string(p_equal_char + 1);
                    break;
                }
            }
        }
    }
    return result;
}
