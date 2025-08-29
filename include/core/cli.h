#ifndef __CORE_CLI_H__
#define __CORE_CLI_H__

#include <core/types.h>
#include <core/error_report.h>

enum {
    OPT_NO_ERR = 0,
    OPT_ERR_INVALID_OPT,
    OPT_ERR_EXPECTED_SUBOPT,
    OPT_ERR_INVALID_SHORT_OPT, /* longer than 1 char */
};

#define REQUIRED_ARGUMENT 1
#define NO_ARGUMENT 0

#define INVALID_OPT_VAL -1
typedef struct {
    usz     c;
    char    **v;
} CLI_Args;

/* definition for option value */
typedef struct {
    u32 id;
    /* short option value (E.g.: '-o') */
    char shortOpt;
    /* long option value (E.g.: '--output' */
    char *longOpt;
    /* select for sub option */
    b32 hasArg;
    /* flag description (E.g.: 'sets a custom path for the output file') */
    char *desc;
} CLI_Opt;

/* definition for 'CLI_Getopt' result */
typedef struct  {
    /* unique enum value */
    u32 id;
    /* error code */
    sz errCode;
    /* error message */
    ErrorReport errReport;
    /* the argument of the option, if supported */
    char *arg;
    /* if the argument was supplied earlier, the index is stored here */
    sz lastIndex;
    /* last cli parameter index */
    sz optInd;
} CLI_OptResult;

/* sentinel initializer for 'Opt_Result' */
#define INIT_OPT_RESULT_INVALID {       \
    .id = -1,                           \
    .errCode = -1,                      \
    .errReport = {0},                   \
    .arg = NULL,                        \
    .lastIndex = -1,                    \
    .optInd = -1                        \
}

/* sentinel initializer for 'Opt' */
#define INIT_OPT_INVALID {          \
    .id = -1,                       \
    .shortOpt = INVALID_OPT_VAL,    \
    .longOpt = NULL,                \
    .hasArg = 0,                    \
    .desc = NULL,                   \
}

CLI_OptResult CLI_GetOpt(const CLI_Opt optArr[],
                usz nOpts, usz *pCounter, CLI_Args args);

#endif /* __CORE_CLI_H__ */
