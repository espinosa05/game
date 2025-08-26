#ifndef __CORE_ERROR_REPORTING_H__
#define __CORE_ERROR_REPORTING_H__

#include <core/types.h>
#include <core/utils.h>

typedef struct {
    usz     rSize;
    char    *rString;
} ErrorReport;

#define EMPTY_ERROR_REPORT(er) (!(er).rString && !(er).rSize)

void ER_AppendReport(ErrorReport *er, char *reportFmt, ...);
void ER_ClearReport(ErrorReport *er);

#endif /* __CORE_ERROR_REPORTING_H__ */
