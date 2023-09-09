#ifndef _CG_DEFS_H_
#define _CG_DEFS_H_
typedef char CG_BOOL;

#define CG_INFO_LOG_SIZE 512
#define CG_SHADER_MAX_SIZE 1024

#define CG_TRUE 1
#define CG_FALSE 0

typedef char CGByte;
typedef unsigned char CGUByte;

#include <wchar.h>
#ifdef CG_USE_UTF16LE
    typedef wchar_t CGChar;
    #define CG_USE_WCHAR

    #define CG_STRLEN wcslen
    #define CG_SPRINTF(buffer, buffer_size, fmt, ...) swprintf(buffer, buffer_size, fmt, __VA_ARGS__)
    #define CG_STRCPY wcscpy
    #define CG_STRCMP wcscmp
    #define CG_FGETC fgetwc
    #define CG_EOF WEOF
    #define CG_STRLEN wcslen
    #define CG_PRINTF wprintf
    #define CG_VPRINTF vwprintf
    
    #define CGFRead(buff, size, count, file)\
        CG_ERROR_COND_EXIT(fread(buff, size, count, file) != count, -1, CGSTR("Failed to read file at line: %d, with location: %d. with count: %d."), __LINE__, ftell(file), count)

    #define CGSTR(str) L##str
#else
    
    typedef char CGChar;

    #define CG_STRLEN strlen
    #define CG_SPRINTF(buffer, buffer_size, fmt, ...) sprintf(buffer, fmt, __VA_ARGS__)
    #define CG_STRCPY strcpy
    #define CG_STRCMP strcmp
    #define CG_FGETC fgetc
    #define CG_EOF EOF
    #define CG_STRLEN strlen
    #define CG_PRINTF printf
    #define CG_VPRINTF vprintf

    #define CGFRead(buff, size, count, file) CG_ERROR_COND_EXIT(fread(buff, size, count, file) != count, -1, CGSTR("Failed to read file at path: %s."), cg_resource_file_path)

    #define CGSTR(str) str
#endif

/********************************************************************************/
/*
 * Key defs. Tese defines are copied from glfw3.h.
 * All the original prefixes, "GLFW," are replaced to "CG" to avoid name conflict.
 * 
 * Copyright (c) 2002-2006 Marcus Geelnard
 * Copyright (c) 2006-2019 Camilla Löwy <elmindreda@glfw.org>
*/
#define CG_KEY_UNKNOWN            -1
#define CG_KEY_SPACE              32
#define CG_KEY_APOSTROPHE         39  /* ' */
#define CG_KEY_COMMA              44  /* , */
#define CG_KEY_MINUS              45  /* - */
#define CG_KEY_PERIOD             46  /* . */
#define CG_KEY_SLASH              47  /* / */
#define CG_KEY_0                  48
#define CG_KEY_1                  49
#define CG_KEY_2                  50
#define CG_KEY_3                  51
#define CG_KEY_4                  52
#define CG_KEY_5                  53
#define CG_KEY_6                  54
#define CG_KEY_7                  55
#define CG_KEY_8                  56
#define CG_KEY_9                  57
#define CG_KEY_SEMICOLON          59  /* ; */
#define CG_KEY_EQUAL              61  /* = */
#define CG_KEY_A                  65
#define CG_KEY_B                  66
#define CG_KEY_C                  67
#define CG_KEY_D                  68
#define CG_KEY_E                  69
#define CG_KEY_F                  70
#define CG_KEY_G                  71
#define CG_KEY_H                  72
#define CG_KEY_I                  73
#define CG_KEY_J                  74
#define CG_KEY_K                  75
#define CG_KEY_L                  76
#define CG_KEY_M                  77
#define CG_KEY_N                  78
#define CG_KEY_O                  79
#define CG_KEY_P                  80
#define CG_KEY_Q                  81
#define CG_KEY_R                  82
#define CG_KEY_S                  83
#define CG_KEY_T                  84
#define CG_KEY_U                  85
#define CG_KEY_V                  86
#define CG_KEY_W                  87
#define CG_KEY_X                  88
#define CG_KEY_Y                  89
#define CG_KEY_Z                  90
#define CG_KEY_LEFT_BRACKET       91  /* [ */
#define CG_KEY_BACKSLASH          92  /* \ */
#define CG_KEY_RIGHT_BRACKET      93  /* ] */
#define CG_KEY_GRAVE_ACCENT       96  /* ` */
#define CG_KEY_WORLD_1            161 /* non-US #1 */
#define CG_KEY_WORLD_2            162 /* non-US #2 */
#define CG_KEY_ESCAPE             256
#define CG_KEY_ENTER              257
#define CG_KEY_TAB                258
#define CG_KEY_BACKSPACE          259
#define CG_KEY_INSERT             260
#define CG_KEY_DELETE             261
#define CG_KEY_RIGHT              262
#define CG_KEY_LEFT               263
#define CG_KEY_DOWN               264
#define CG_KEY_UP                 265
#define CG_KEY_PAGE_UP            266
#define CG_KEY_PAGE_DOWN          267
#define CG_KEY_HOME               268
#define CG_KEY_END                269
#define CG_KEY_CAPS_LOCK          280
#define CG_KEY_SCROLL_LOCK        281
#define CG_KEY_NUM_LOCK           282
#define CG_KEY_PRINT_SCREEN       283
#define CG_KEY_PAUSE              284
#define CG_KEY_F1                 290
#define CG_KEY_F2                 291
#define CG_KEY_F3                 292
#define CG_KEY_F4                 293
#define CG_KEY_F5                 294
#define CG_KEY_F6                 295
#define CG_KEY_F7                 296
#define CG_KEY_F8                 297
#define CG_KEY_F9                 298
#define CG_KEY_F10                299
#define CG_KEY_F11                300
#define CG_KEY_F12                301
#define CG_KEY_F13                302
#define CG_KEY_F14                303
#define CG_KEY_F15                304
#define CG_KEY_F16                305
#define CG_KEY_F17                306
#define CG_KEY_F18                307
#define CG_KEY_F19                308
#define CG_KEY_F20                309
#define CG_KEY_F21                310
#define CG_KEY_F22                311
#define CG_KEY_F23                312
#define CG_KEY_F24                313
#define CG_KEY_F25                314
#define CG_KEY_KP_0               320
#define CG_KEY_KP_1               321
#define CG_KEY_KP_2               322
#define CG_KEY_KP_3               323
#define CG_KEY_KP_4               324
#define CG_KEY_KP_5               325
#define CG_KEY_KP_6               326
#define CG_KEY_KP_7               327
#define CG_KEY_KP_8               328
#define CG_KEY_KP_9               329
#define CG_KEY_KP_DECIMAL         330
#define CG_KEY_KP_DIVIDE          331
#define CG_KEY_KP_MULTIPLY        332
#define CG_KEY_KP_SUBTRACT        333
#define CG_KEY_KP_ADD             334
#define CG_KEY_KP_ENTER           335
#define CG_KEY_KP_EQUAL           336
#define CG_KEY_LEFT_SHIFT         340
#define CG_KEY_LEFT_CONTROL       341
#define CG_KEY_LEFT_ALT           342
#define CG_KEY_LEFT_SUPER         343
#define CG_KEY_RIGHT_SHIFT        344
#define CG_KEY_RIGHT_CONTROL      345
#define CG_KEY_RIGHT_ALT          346
#define CG_KEY_RIGHT_SUPER        347
#define CG_KEY_MENU               348

#define CG_MOUSE_BUTTON_1         0
#define CG_MOUSE_BUTTON_2         1
#define CG_MOUSE_BUTTON_3         2
#define CG_MOUSE_BUTTON_4         3
#define CG_MOUSE_BUTTON_5         4
#define CG_MOUSE_BUTTON_6         5
#define CG_MOUSE_BUTTON_7         6
#define CG_MOUSE_BUTTON_8         7
#define CG_MOUSE_BUTTON_LAST      CG_MOUSE_BUTTON_8
#define CG_MOUSE_BUTTON_LEFT      CG_MOUSE_BUTTON_1
#define CG_MOUSE_BUTTON_RIGHT     CG_MOUSE_BUTTON_2
#define CG_MOUSE_BUTTON_MIDDLE    CG_MOUSE_BUTTON_3

#define CG_RELEASE                0
#define CG_PRESS                  1
#define CG_REPEAT                 2

/********************************************************************************/

// define target platform
#if (defined __WIN32__) || (defined _WIN32)
    #define CG_TG_WIN              //windows
    #ifdef _WIN64
        #define CG_TG_WIN_64       //windows 64
    #else
        #define CG_TG_WIN_32       //windows 32
    #endif // _WIN64
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        #define CG_TG_MACOS        //mac os
    #else
        #error "Unknown target platform."
    #endif
#elif __linux__
    #define CG_TG_LINUX              //linux
#elif __unix__
    #error "Unix is not supported for CosGraphics."
#elif __ANDROID__
    #error "Android is not supported for CosGraphics."
#else
    #error "Unknown target platform."
#endif

#endif

