#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#undef CORE_PLATFORM_I386
#undef CORE_PLATFORM_AMD64
#undef CORE_PLATFORM_HOSTED_ENVIRONMENT	/* TODO IMPLEMENT BAREMETAL */

#undef CORE_PLATFORM_WINDOWS
#undef CORE_PLATFORM_WINDOWS_AMD64
#undef CORE_PLATFORM_WINDOWS_X86
#undef CORE_PLATFORM_WINDOWS_ARM64
#undef CORE_PLATFORM_UNIXLIKE
#undef CORE_PLATFORM_LINUX
#undef CORE_PLATFORM_MOBILE
#undef CORE_PLATFORM_MOBILE_ANDROID
#undef CORE_PLATFORM_MOBILE_IOS
#undef CORE_PLATFORM_MACOS
#undef CORE_PLATFORM_BSD

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   define CORE_PLATFORM_WINDOWS            (1)
#   ifdef __WIN64
#       define CORE_PLATFORM_WINDOWS_AMD64  (1)
#   else
#       define CORE_PLATFORM_WINDOWS_X86    (1)
#   endif /* __WIN64 */
#   ifdef _M_ARM64
#       define CORE_PLATFORM_WINDOWS_ARM64  (1)
#   endif /* _M_ARM64 */
#elif __unix__
#   define CORE_PLATFORM_UNIXLIKE           (1)
#   ifdef __linux__
#       define CORE_PLATFORM_LINUX          (1)
#   elif __ANDROID__
#       ifdef __BUILD_TARGET_MOBILE_ANDROID /* compiler flag definition */
#           define CORE_PLATFORM_MOBILE     (1)
#       endif /* __BUILD_TARGET_MOBILE_ANDROID */
#   elif __APPLE__
#       include "TargetConditionals.h"
#       ifdef TARGET_OS_IPHONE
#           define CORE_PLATFORM_MOBILE     (1)
#           define CORE_PLATFORM_MOBILE_IOS (1)
#       elif TARGET_OS_MAC
#           define CORE_PLATFORM_MACOS      (1)
#       endif /* TARGET_OS_IPHONE */
#   elif BSD
#       define CORE_PLATFORM_BSD            (1)
#   endif /* __linux__ */
#else
#   error "what are you even trying to target bro..."
#endif /* WIN32, _WIN32, __WIN32__, __NT__ */

#endif /* __PLATFORM_H__ */
