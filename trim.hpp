#ifndef TRIM_H
#define TRIM_H

#include <iostream>
#include <string>

extern bool newlineFlag;

template <typename T>
void print(const T& stuff) {
    std::cout << stuff << std::endl;
}

int clean(const std::string &file_name);


#endif  // TRIM_H