#include <core/cli.h>
#include <core/log.h>
#include <core/utils.h>

#include <core/cstd.h>
#include <string.h>

/* static function declaration start */
static CLI_Opt LoadLongOpt(const char *arg, const CLI_Opt opts[], usz count);
static CLI_Opt LoadShortOpt(const char *arg, const CLI_Opt opts[], usz count);
static b32 IsLastOpt(const CLI_Opt opt);
/* static function declaration end */


CLI_OptResult CLI_Getopt(const CLI_Opt optArr[], usz nOpts, usz *pCounter, CLI_Args args)
{
    CLI_OptResult   res = INIT_OPT_RESULT_INVALID;
    CLI_Opt         optEntry = {0};
    const char      *arg = NULL;
    usz             *counter = NULL;

    ASSERT_RT(pCounter, "counter needs to be 0");

    ASSERT_RT(IsLastOpt(optArr[nOpts - 1]),
              "corrupt data! last entry of CLI_opt array needs to be {0}");

    ASSERT_RT(args.c >= *counter,
              "index not inside argument range! (%d)", *counter);

    res.optInd = *counter;
    arg = args.v[*counter];

    if (arg[0] != '-') {
        res.errCode = OPT_ERR_INVALID_OPT;
        ER_AppendReport(&res.errReport, "option '%s' is not an option!", arg);
        return res;
    }

    /* consume current arg */
    (*counter)++;

    /* is long argument? (double dash) */
    if (arg[1] == '-') {
        optEntry = LoadLongOpt(arg, optArr, nOpts);
    } else {
        optEntry = LoadShortOpt(arg, optArr, nOpts);
    }

    res.id = optEntry.id;

    if (optEntry.hasArg) {
        if (args.c >= *counter + 1) {
            res.optArg = args.v[*counter];
            (*counter)++; /* skip option */
        } else {
            res.errCode = OPT_ERR_EXPECTED_SUBOPT;
            ER_AppendReport(&res.errReport, "option '%s' requires an argument", arg);
        }
    }
    return res;
}

/* constatns for skipping the dashes */
enum argOffsets {
    SHORT_ARG_OFFSET = 1,
    LONG_ARG_OFFSET = 2,
};

static CLI_Opt LoadLongOpt(const char *arg, const CLI_Opt opts[], usz count)
{
    CLI_Opt ret = INIT_OPT_INVALID;

    arg += LONG_ARG_OFFSET;

    for (usz i = 0; i < count; ++i) {
        if (0 == strcmp(opts[i].longOpt, arg)) {
            ret = opts[i];
            break;
        }
    }

    return ret;
}

static CLI_Opt LoadShortOpt(const char *arg, const CLI_Opt opts[], usz count)
{
    CLI_Opt ret = INIT_OPT_INVALID;
    char shortArg;

    arg += SHORT_ARG_OFFSET;
    shortArg = arg[0];

    for (usz i = 0; i < count; ++i) {
        if (shortArg == opts[i].shortOpt) {
            ret = opts[i];
            break;
        }
    }
    return ret;
}

static b32 IsLastOpt(const CLI_Opt opt)
{
    return (opt.shortOpt == 0
            && opt.longOpt == NULL
            && opt.hasArg == 0
            && opt.desc == NULL);
}


