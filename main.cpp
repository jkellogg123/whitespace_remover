#include <iostream>
#include "trim.hpp"
#include "argparse.hpp"

bool newlineFlag = false;

int main_real(int argc, char* argv[]) {
    argparse::ArgumentParser parser("trimmer", "0.2.0");

    parser.add_argument("-n", "--newline")
           .help("ignore newlines (will clean everything to a single line if possible)")
           .flag();
    
    parser.add_argument("file")
            .help("Name of file to be processed")
            .required();

    std::string file_name;
    try {
        parser.parse_args(argc, argv);
        newlineFlag = parser.get<bool>("-n");
        file_name = parser.get<std::string>("file");
    } catch (const std::exception& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        std::cout << parser;
        return 1;
    }

    int result = clean(file_name);
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