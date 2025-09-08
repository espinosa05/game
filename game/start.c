#include <core/cli.h>

extern void Main(CLI_Args args);

int main(int argc, char **argv)
{
    CLI_Args args = {argc, argv};
    Main(args);

    return EXIT_SUCCESS;
}
