#include "cpp_trim.hpp"

#include <iostream>
#include <fstream>
#include <cctype>       // isspace
#include <sstream>
#include <cassert>
#include <bitset>
#include "trim.hpp"

bool is_special_char(char c) {
    static std::bitset<256> specials;
    if (specials.none()) {
        for (char s : "(){}[]<>+-/*&^%;") {
            specials.set(s);
        }
    }
    return specials[c];
}

void clean_line(std::string &line) {
    trim(line);
    if (line.size() < 2) {
        return;
    }

    std::ostringstream new_line;
    new_line << line[0];
    bool prev_iswhite = false;
    char last_char = line[0];

    for (charit_t i = 1; i < line.size(); ++i) {
        if (std::isspace(line[i])) {
            prev_iswhite = true;
            continue;
        }
        else {
            // if (prev_iswhite) {
            // change logic so that it doesn't modify strings
            if (prev_iswhite && !is_special_char(last_char) && !is_special_char(line[i])) {
                new_line << ' ';
            }
            prev_iswhite = false;
            new_line << line[i];
            last_char = line[i];
        }
    }
    line = new_line.str();
}

bool is_cpp(const std::string &file_name) {
    if (file_name.size() < 4) {
        return false;
    }
    return file_name.substr(file_name.size() - 4) == ".cpp";
}

int cpp_clean(const std::string &file_name) {
    assert(is_cpp(file_name));

    std::ifstream in_file(file_name);
    if (!in_file.is_open()) {
        std::cerr << "Error opening in_file!" << std::endl;
        return 1;
    }
    std::ofstream out_file("output/(CLEAN)" + file_name);
    if (!out_file.is_open()) {
        in_file.close();
        std::cerr << "Error opening out_file!" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(in_file, line)) {
        clean_line(line);
        if (line.size() > 0)
            out_file << line << std::endl;
    }

    in_file.close();
    out_file.close();

    return 0;
}