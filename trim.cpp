#include "trim.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include "trim.hpp"

void trim_l(std::string &str) {
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
void trim_r(std::string &str) {
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
void trim(std::string &str) {
    trim_l(str);
    trim_r(str);
}