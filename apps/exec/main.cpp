#include "exec.hpp"

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        exec(argv[i]);
    }
}