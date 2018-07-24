#include <iostream>
#include "Chunk.h"


int main(int argc, const char** argv)
{
    Chunk c;
    c.write('a');

    for (int i = 0; i < c.count; ++i) {
        std::cout << c.code[i] << '\n';
    }

    c.free();
    return 0;
}

