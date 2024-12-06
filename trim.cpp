#include "trim.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>   // std::function
#include <fstream>
#include <sstream>
#include <bitset>
#include <filesystem>   // check if directory/file exists
#include <cctype>       // std::isspace

typedef int charit_t;

const std::string output_folder = "./trimmer_output";

int clean_cpp(std::ifstream&, std::ofstream&);
int clean_txt(std::ifstream&, std::ofstream&);
std::unordered_map<std::string, std::function<int(std::ifstream&, std::ofstream&)>> ext_to_func = {
    {".cpp", clean_cpp},
    {".hpp", clean_cpp},
    {".c", clean_cpp},
    {".h", clean_cpp},
    // {".txt", clean_txt},
    {".txt", clean_cpp},
};


// returns "" if not valid extension
std::string get_extension(const std::string& file_name) {
    for (const auto& [ext, _] : ext_to_func) {
        if ((ext.size() <= file_name.size()) && file_name.ends_with(ext))
            return ext;
    }
    return "";
}

// true if successful, false if not (only false if trying to create folder fails)
bool create_output() {
    if (!std::filesystem::exists(output_folder)) {
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
        for (char s : "(){}[]<>+-/*&^%;:=,") {
            specials.set(s);
        }
    }
    return specials[c];
}

// Special case for cpp for something like the following:
// typename std::enable_if<std::is_integral<T>::value>::type*=nullptr  (need space between * and =)
// template<>constexpr bool thing<int>=true;          (interprets <int>=true as <int >= true)
// (these are both in argparse.hpp)
bool equal_and_op(char c1, char c2) {
    return c1 == '=' && (c2 == '+' || c2 == '-' || c2 == '*' || c2 == '/' || c2 == '<' || c2 == '>');
}

bool needs_space(char c1, char c2) {
    return !(c1 == '\n' || c2 == '\n') && (!is_special_char(c1) && !is_special_char(c2)) || equal_and_op(c1, c2) || equal_and_op(c2, c1);
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
            // TODO: change logic so that it doesn't modify strings
            if (prev_iswhite && needs_space(last_char, line[i])) {
                new_line << ' ';
            }
            prev_iswhite = false;
            new_line << line[i];
            last_char = line[i];
        }
    }
    line = new_line.str();
}

bool starts_with_hash(std::string& str) {
    for (char c : str) {
        if (std::isspace(c))
            continue;
        else if (c == '#')
            return true;
        else
            return false;
    }
    return false;
}

void clean_cpp_newline(std::ifstream& in_file, std::ofstream& out_file) {
    char c;
    char last_char = '\0';
    bool prev_iswhite = false;
    bool in_string = false;
    while (in_file.get(c)) {
        // want to keep strings intact
        // NOTE: Currently doesn't account for multiline strings of any kind
        if (c == '"' && last_char != '\\') {
            out_file << c;
            in_string = !in_string;
            last_char = c;
            continue;
        }
        if (in_string) {
            out_file << c;
            last_char = c;
            continue;
        }
        // ' characters for chars make things more complicated
        if (c == '\'') {
            out_file << c;
            in_file.get(c);
            out_file << c;
            if (c == '\\') {
                in_file.get(c);
                out_file << c;
            }
            in_file.get(c);     // should be closing '
            out_file << c;
            continue;
        }
        // #includes and comments need special treatment
        if (c == '#' || (c == '/' && last_char == '/')) {
            std::string line;
            std::getline(in_file, line);
            clean_line(line);
            if (c == '/') {
                out_file << c << line << '\n';
            } else {
                out_file << (last_char == '\n' ? "#" : "\n#") << line << '\n';
            }
            prev_iswhite = true;
            last_char = '\n';
            continue;
        }

        if (std::isspace(c)) {
            prev_iswhite = true;
            continue;
        }
        if (prev_iswhite && needs_space(last_char, c)) {
            out_file << ' ';
        }
        prev_iswhite = false;
        out_file << c;
        last_char = c;
    }
}

int clean_cpp(std::ifstream& in_file, std::ofstream& out_file) {
    if (newlineFlag) {
        clean_cpp_newline(in_file, out_file);
        return 0;
    }
    std::string line;
    while (std::getline(in_file, line)) {
        clean_line(line);
        if (line.size() > 0)
            out_file << line << '\n';
    }

    return 0;
}

int clean_txt(std::ifstream& in_file, std::ofstream& out_file) {
    std::string line;
    while (std::getline(in_file, line)) {
        clean_line(line);
        if (line.size() > 0)
            out_file << line << std::endl;
    }

    return 0;
}

int clean(const std::string& file_name) {
    if (!std::filesystem::exists(file_name)) {
        std::cerr << "Couldn't find file \'" << file_name << "\'" << std::endl;
        return 1;
    }

    std::string ext = get_extension(file_name);
    if (ext.empty()) {
        std::cerr << "Invalid file extension!" << std::endl;
        return 1;
    }

    if (!create_output()) {
        std::cerr << "Couldn't create output folder!" << std::endl;
        return 1;
    }
    std::ifstream in_file(file_name);
    if (!in_file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    std::ofstream out_file(output_folder + "/CLEAN_" + file_name);
    if (!out_file.is_open()) {
        in_file.close();
        std::cerr << "Error creating output file!" << std::endl;
        return 1;
    }    

    return ext_to_func[ext](in_file, out_file);
}
