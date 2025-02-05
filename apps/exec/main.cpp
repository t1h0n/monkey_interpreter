#include <mlang/exec.hpp>

auto main(int argc, char* argv[]) -> int
{
    for (int i = 1; i < argc; ++i)
    {
        mlang::exec(argv[i]);
    }
}
