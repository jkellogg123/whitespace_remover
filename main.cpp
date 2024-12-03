#include <iostream>
#include "trim.hpp"

int main_real(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Proper usage is: ./script <file (optional)>" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    int result = cpp_clean(file_name);

    if (!result) {
        print("Success!");
    }

    return result;
}

int main_test(int argc, char* argv[]) {
    print("testing...");

    return 0;
}

int main(int argc, char* argv[]) {
    return main_real(argc, argv);
    // return main_test(argc, argv);
}