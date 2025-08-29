#define APP_IMPL
#include "app.h"

#define LOW_GERMAN_GREETING "Moin Yan, wo geiht di dat? :^)"

void Main(CLI_Args args)
{
    App_Context app = {0};
    App_ContextCreateInfo appInfo = {
        .args = args,
        .name = LOW_GERMAN_GREETING,
    };

    App_Init(&app, &appInfo);
    App_Run(&app);
    App_Cleanup(&app);
}
