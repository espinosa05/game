#include <core/error_report.h>
#include <core/strings.h>
#include <core/variadic.h>
#include <core/memory.h>

void ER_ClearReport(ErrorReport *er)
{
    M_Free(er->rString);
    M_Zero(er, sizeof(*er));
}

/* very simple string builder, might extract this to it's own definition later (which means never) */
void ER_AppendReport(ErrorReport *er, char *reportFmt, ...)
{
    VA_List ap;
    char *report = NULL;

    VA_Start(ap, reportFmt);
    usz reportLen = vasprintf(&report, reportFmt, ap);

    if (!er->rString) {
        er->rSize = reportLen + NULL_TERM_SIZE;
        er->rString = M_Alloc(BYTE_SIZE, er->rSize);
    } else {
        er->rSize += reportLen + NULL_TERM_SIZE;
        er->rString = M_Realloc(er->rString, BYTE_SIZE, er->rSize);
    }

    strncat(er->rString, report, reportLen);
    NULL_TERM_BUFF(er->rString, er->rSize);
    M_Free(report);
}

