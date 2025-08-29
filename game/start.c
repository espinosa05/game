#include <core/cli.h>
#include "game.h"
#include "cdstruct.c"

int main(int argc, char **argv)
{

    CSTRUCT();

    CLI_Args args = {argc, argv};
    Main(args);

    DSTRUCT();

    return EXIT_SUCCESS;
}
