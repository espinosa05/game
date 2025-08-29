#include <core/cli.h>
#include "game.h"

int main(int argc, char **argv)
{

    CLI_Args args = {argc, argv};
    Main(args);

    return EXIT_SUCCESS;
}
