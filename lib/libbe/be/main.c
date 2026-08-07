#include <be/be_engine.h>

int main(int argc, char **argv)
{
    BeEngine be = {0};
    be_engine_init(&be, CLI_ARGS(argc, argv));
    be_engine_run(&be);
    be_engine_delete(&be);

    return OS_EXIT_SUCCESS;
}
