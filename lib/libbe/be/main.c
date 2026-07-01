#include <be/be_engine.h>

int main(void)
{
    struct be_engine be_engine = {0};

    be_engine_init(&be_engine);
    be_engine_run(&be_engine);
    be_engine_delete(&be_engine);
}
