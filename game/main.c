#include "app.h"

/* i know you don't speak Low German, but close enough */
#define LOW_GERMAN_GREETING "Moin Yan, wo geiht di dat? :^)"

int main(int argc, char **argv)
{
    struct cli_args args = {argc, argv};
    struct app = {0};
    struct app_info = {
        .args = args,
        .name = LOW_GERMAN_GREETING,
    };

    app_init(&app, &app_info);
    app_run(&app);
    app_cleanup(&app);

    return EXIT_SUCCESS;
}
