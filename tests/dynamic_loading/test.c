#include <core/utils.h>
#include <core/os_dynamic_library.h>
#include <core/log.h>

typedef void (*Function_Pointer)(void);

Function_Pointer start;
Function_Pointer run;
Function_Pointer shutdown;

OS_Library testLib;
OS_LibraryCreaetInfo testLibInfo;

void InitFunctions(void)
{
    start       = NULL;
    run         = NULL;
    shutdown    = NULL;
    LOG_INFO("init test...");

    testLibInfo.path = "./";
    testLibInfo.name = "test";
    OS_OpenLibrary(&testLib, &testLibInfo);
    OS_LoadLibrarySymbol(&testLib, &start,      "TestStartFunc");
    OS_LoadLibrarySymbol(&testLib, &run,        "TestRunFunc");
    OS_LoadLibrarySymbol(&testLib, &shutdown,   "TestShutdownFunc");

    ASSERT(start,       "start function NULL!!");
    ASSERT(run,         "run function NULL!!");
    ASSERT(shutdown,    "shutdown function NULL!!");
}

void RunFunctions(void)
{
    LOG_INFO("running test...");
    start();
    run();
    shutdown();
    LOG_INFO("test complete!");
}

void Cleanup(void)
{
    LOG_INFO("finishing test...");
    OS_CloseLibrary(&testLib);
}

int main(void)
{
    InitFunctions();
    RunFunctions();
    Cleanup();

    return 0;
}
