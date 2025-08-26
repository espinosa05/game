#ifndef __CORE_NATIVE_UI_H__
#define __CORE_NATIVE_UI_H__

#include <core/error_report.h>

#define GUI_ASSERT(assertion, ...)                                              \
    if (!(assertion)) {                                                         \
        IMPL_AssertGUI(#assertion, __LINE__, __FILE__, __func__, __VA_ARGS__);  \
    }

void IMPL_GUIAssert(char *assertString, usz line, char *file, char *func,
                    char *msg, char *detailedErrorFmt, ...);

#endif /* __CORE_NATIVE_UI_H__ */
