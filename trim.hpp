#ifndef TRIM_H
#define TRIM_H

#include <iostream>
#include <string>

typedef int charit_t;

template <typename T>
void print(const T &stuff) {
    std::cout << stuff << std::endl;
}

void trim(std::string &line);

#endif  // TRIM_H