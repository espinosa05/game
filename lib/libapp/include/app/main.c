#include "be.h"

int main(int argc, char **argv)
{
    struct be_app app = {0};
    be_app_init(&app);
    be_app_run(&app);
    be_app_delete(&app);

    return OS_EXIT_SUCCESS;
}
