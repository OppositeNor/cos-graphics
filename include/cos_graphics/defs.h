#ifndef _CG_DEFS_H_
#define _CG_DEFS_H_
typedef char CG_BOOL;

#define CG_INFO_LOG_SIZE 512
#define CG_SHADER_MAX_SIZE 1024

#define CG_TRUE 1
#define CG_FALSE 0

// define target platform
#ifdef __WIN32__
    #define CG_TG_WIN              //windows
    #ifdef _WIN649
        #define CG_TG_WIN_64       //windows 64
    #else
        #define CG_TG_WIN_32       //windows 32
    #endif // _WIN64
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #error "IPhone Simulator is not supported for CosGraphics"
    #elif TARGET_OS_IPHONE
        #error "IPhone is not supported for CosGraphics"
    #elif TARGET_OS_MAC
        #define CG_TG_MACOS        //mac os
    #else
        #error "Unknown target platform"
    #endif
#elif __linux__
    #define CG_TG_LINUX              //linux
#elif __unix__
    #error "Unix is not supported for CosGraphics"
#elif __ANDROID__
    #error "Android is not supported for CosGraphics"
#else
    #error "Unknown target platform"
#endif
#endif