#include "trim.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>   // std::function
#include <fstream>
#include <sstream>
#include <cassert>
#include <bitset>
#include <filesystem>   // check if directory exists
#include <cctype>       // std::isspace

std::string output_folder = "./trimmer_output";

int clean_cpp(const std::string&);
int clean_txt(const std::string&);

std::unordered_map<std::string, std::function<int(const std::string&)>> ext_to_func = {
    {".cpp", clean_cpp},
    {".txt", clean_txt},
};


// returns "" if not valid extension
std::string get_extension(const std::string& file_name) {
    for (const auto& [ext, _] : ext_to_func) {
        if ((ext.size() <= file_name.size()) && (file_name.substr(file_name.size() - ext.size()) == ext))
            return ext;
    }
    return "";
}

// true if successful, false if not (only false if trying to create folder fails)
bool create_output() {
    if (!std::filesystem::exists(output_folder)) {
        print("Creating output folder...");
        return std::filesystem::create_directory(output_folder);
    }
    return true;
}

void trim_l(std::string& str) {
    charit_t count = 0;
    for (charit_t i = 0; i < str.size(); ++i) {
        if (std::isspace(str[i])) {
            ++count;
        }
        else
            break;
    }
    str.erase(0, count);
}
void trim_r(std::string& str) {
    charit_t count = 0;
    for (charit_t i = str.size() - 1; i >= 0; --i) {
        if (std::isspace(str[i])) {
            ++count;
        }
        else
            break;
    }
    str.erase(str.size() - count, count);
}
void trim(std::string& str) {
    trim_l(str);
    trim_r(str);
}

bool is_special_char(char c) {
    static std::bitset<256> specials;
    if (specials.none()) {
        for (char s : "(){}[]<>+-/*&^%;") {
            specials.set(s);
        }
    }
    return specials[c];
}

void clean_line(std::string& line) {
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

int clean_cpp(const std::string& file_name) {
    std::ifstream in_file(file_name);
    if (!in_file.is_open()) {
        std::cerr << "Error opening in_file!" << std::endl;
        return 1;
    }
    if (!create_output()) {
        std::cerr << "Couldn't create output folder!" << std::endl;
        return 1;
    }
    std::ofstream out_file(output_folder + "/(CLEAN)" + file_name);
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

int clean_txt(const std::string& file_name) {
    std::ifstream in_file(file_name);
    if (!in_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    if (!create_output()) {
        std::cerr << "Couldn't create output folder!" << std::endl;
        return 1;
    }
    std::ofstream out_file(output_folder + "/(CLEAN)" + file_name);
    if (!out_file.is_open()) {
        in_file.close();
        std::cerr << "Error creating output file!" << std::endl;
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

int clean(const std::string& file_name) {
    std::string ext = get_extension(file_name);
    if (ext.empty()) {
        std::cerr << "Invalid file extension!" << std::endl;
        return 1;
    }
    return ext_to_func[ext](file_name);
}
