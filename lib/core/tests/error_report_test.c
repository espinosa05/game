#include <stdio.h>
#include <core/error_report.h>

int main(int argc, char **argv)
{
    ErrorReport er = {0};
    ER_AppendReport(&er, "[ERROR] TEST_ERROR %s\n", *argv);
    ER_AppendReport(&er, "[FATAL] FATAL TEST ERROR %d\n", argc);

    printf("error report:\n%s", er.rString);
    ER_ClearReport(&er);
    ER_AppendReport(&er, "[test]\n");

    printf("error report:\n%s", er.rString);
}
