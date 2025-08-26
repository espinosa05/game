#include "game.h"
#include <core/cli.h>

#include "cdstruct.c"

int main(int argc, char **argv)
{
    for (usz i = 0; i < ARRAY_COUNT(initList); ++i)
        initList[i]();

    CLI_Args args = {argc, argv};
    Main(&args);

    for (usz i = 0; i < ARRAY_COUNT(exitList); ++i)
        exitList[i]();

    return EXIT_SUCCESS;
}
