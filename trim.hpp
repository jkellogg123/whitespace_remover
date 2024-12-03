#ifndef TRIM_H
#define TRIM_H

#include <iostream>
#include <string>

typedef int charit_t;

template <typename T>
void print(const T &stuff) {
    std::cout << stuff << std::endl;
}

int cpp_clean(const std::string &file_name);


#endif  // TRIM_H