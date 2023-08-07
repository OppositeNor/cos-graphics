#ifndef _CGRW_RESOURCE_WRAPPER_H_
#define _CGRW_RESOURCE_WRAPPER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define CGRW_BOOL char
#define CGRW_TRUE 1
#define CGRW_FALSE 0

#define CGRW_TYPE_BUFF_SIZE 64
#define CGRW_KEY_BUFF_SIZE 64
#define CGRW_PATH_BUFF_SIZE 128

// define target platform
#if (defined __WIN32__) || (defined _WIN32)
    #define CGRW_TG_WIN              //windows
    #ifdef _WIN64
        #define CGRW_TG_WIN_64       //windows 64
    #else
        #define CGRW_TG_WIN_32       //windows 32
    #endif // _WIN64
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        #define CGRW_TG_MACOS        //mac os
    #else
        #error "Unknown target platform."
    #endif
#elif __linux__
    #define CGRW_TG_LINUX              //linux
#elif __unix__
    #error "Unix is not supported for CosGraphics."
#elif __ANDROID__
    #error "Android is not supported for CosGraphics."
#else
    #error "Unknown target platform."
#endif

#include <stdio.h>


#include <wchar.h>
#ifdef CGRW_USE_WCHAR
    #ifdef CGRW_TG_WIN
        typedef wchar_t CGRWChar;

        #define CGRW_STRLEN wcslen
        #define CGRW_SPRINTF(buffer, buffer_size, fmt, ...) swprintf(buffer, buffer_size, fmt, __VA_ARGS__)
        #define CGRW_STRCPY wcscpy
        #define CGRW_STRCMP wcscmp
        #define CGRW_FGETC fgetwc
        #define CGRW_EOF WEOF
    #else
        #warning "CGRW wide character is not supported for your current platform. Using char instead."
        typedef char CGRWChar;
        #define CGRW_SPRINTF(buffer, buffer_size, fmt, ...) sprintf(buffer, fmt, __VA_ARGS__)

        #define CGRW_STRLEN strlen
        #define CGRW_STRCPY strcpy
        #define CGRW_STRCMP strcmp
        #define CGRW_FGETC fgetc
        #define CGRW_EOF EOF

        #undef CGRW_USE_WCHAR
    #endif
#else
    typedef char CGRWChar;
    #define CGRW_SPRINTF(buffer, buffer_size, fmt, ...) sprintf(buffer, fmt, __VA_ARGS__)

    #define CGRW_STRLEN strlen
    #define CGRW_STRCPY strcpy
    #define CGRW_STRCMP strcmp
    #define CGRW_FGETC fgetc
    #define CGRW_EOF EOF


#endif
/**
 * @brief Cast CGRWChar to char.
 * 
 * @param str The string to be casted.
 * @param buffer The buffer to store the result.
 */
void CGRWCharToChar(const CGRWChar* str, char* buffer);

/**
 * @brief Cast char to CGRWChar.
 * 
 * @param str The string to be casted.
 * @param buffer The buffer to store the result.
 */
void CharToCGRWChar(const char* str, CGRWChar* buffer);

#ifdef CGRW_USE_WCHAR
    static inline FILE* CGRWFOpen(const CGRWChar* file_path, const char* mode)
    {
        char buffer[256];
        CGRWCharToChar(file_path, buffer);
        return fopen(buffer, mode);
    }

    static inline void CGRWRemove(const CGRWChar* file_path)
    {
        char buffer[256];
        CGRWCharToChar(file_path, buffer);
        remove(buffer);
    }
#else
    static inline FILE* CGRWFOpen(const CGRWChar* file_path, const char* mode)
    {
        return fopen(file_path, mode);
    }

    static inline void CGRWRemove(const CGRWChar* file_path)
    {
        remove(file_path);
    }
#endif

typedef char* CGRWResourceIdentifier;

/**
 * @brief The resource data gathered from the ures file
 */
typedef struct CGRWResourceData
{
    CGRWChar type[CGRW_TYPE_BUFF_SIZE];     /*The type of the resource.*/
    CGRWChar key[CGRW_KEY_BUFF_SIZE];      /*The key of the resource.*/
    CGRWChar path[CGRW_PATH_BUFF_SIZE];     /*The path of the resource.*/
    struct CGRWResourceData* next; /*The next resource data.*/
}CGRWResourceData;

/**
 * @brief Initialize CG resource wrapper
 * 
 */
void CGRWInit(int argc, char* argv[]);

/**
 * @brief Add resource to resource file.
 * 
 * @param path The path of the resource.
 * @param key The key of the resource.
 * @return CGRWResourceIdentifier The identifier of the resource.
 */
void CGRWAddResource(CGRWResourceData* res_data);

/**
 * @brief Load the resource from disk
 * 
 * @param resource_key The resource key
 * @return char* The loaded data. This needs to be freed manually.
 */
CGRWChar* CGRWLoadResource(const CGRWChar* resource_key);

CGRWResourceData* CGRWPhraseUsedResource(const CGRWChar* file_path);

/**
 * @brief Terminate CG resource wrapper.
 */
void CGRWTerminate();

#ifdef __cplusplus
}
#endif
#endif  // _CGRW_RESOURCE_WRAPPER_H_