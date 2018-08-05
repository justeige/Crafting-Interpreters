#include <iostream>
#include <string>
#include <fstream>

#include "Common.h"
#include "Chunk.h"
#include "Debug.h"
#include "VM.h"


void repl(VM& vm);
void run_file(VM& vm, const char* file_name);

int main(int argc, const char** argv)
{
    VM vm;

    if (argc == 1) {
        repl(vm);
    }
    else if (argc == 2) {
        run_file(vm, argv[1]);
    }
    else {
        std::cout << "Usage: clox [path]\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void repl(VM& vm)
{
    forever {
        std::cout << "> ";

        std::string line;
        std::getline(std::cin, line);

        if (line == "exit") {
            return;
        }

        vm.interpret(line);
    }
}

void run_file(VM& vm, const char* file_name)
{
    std::ifstream file;
    file.open(file_name, std::ifstream::in);

    if (!file.is_open()) {
        std::cout << "File '" << file_name << "' not found!\n";
        return;
    }

    std::cout << file_name << " is running...\n";

    std::string line;
    while (std::getline(file, line)) {
        vm.interpret(line);
    }
}