#ifndef __CORE_MACROS_H__
#define __CORE_MACROS_H__

/* used for preprocessor comments */
#define COMMENT 0

#define MACRO_START do {
#define MACRO_END   } while (0)

#define __EMPTY_MACRO__ MACRO_START MACRO_END

/* not required, just to match MACRO_START, MACRO_END for multiline macros */
#define EXPR_MACRO_START    (
#define EXPR_MACRO_END      )

#endif /* __CORE_MACROS_H__ */
