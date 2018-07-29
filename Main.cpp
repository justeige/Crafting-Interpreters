#include <iostream>
#include <string>

#include "Common.h"
#include "Chunk.h"
#include "Debug.h"
#include "VM.h"


void repl(VM& vm);
void run_file(const char* file_name);

int main(int argc, const char** argv)
{
    VM vm;

    if (argc == 1) {
        repl(vm);
    }
    else if (argc == 2) {
        //run_file(argv[1]);
    }
    else {
        std::fprintf(stderr, "Usage: clox [path]\n");
        return -1;
    }

    return 0;
}

void repl(VM& vm)
{
    forever {
        std::string line;

        std::cout << "> ";
        std::getline(std::cin, line);

        vm.interpret(line);
    }
}