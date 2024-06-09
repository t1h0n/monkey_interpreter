#include "mlang/exec.hpp"

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        mlang::exec(argv[i]);
    }
}