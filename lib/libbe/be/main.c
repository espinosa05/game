#include <be/be_engine.h>

int main(int argc, char **argv)
{
    struct be_engine be_engine = {0};

    be_engine_init(&be_engine, CLI_ARGS(argc, argv));
    be_engine_run(&be_engine);
    be_engine_delete(&be_engine);
}
