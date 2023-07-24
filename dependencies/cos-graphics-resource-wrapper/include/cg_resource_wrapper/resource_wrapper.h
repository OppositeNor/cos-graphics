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

typedef char* CGRWResourceIdentifier;

/**
 * @brief The resource data gathered from the ures file
 */
typedef struct CGRWResourceData
{
    char type[CGRW_TYPE_BUFF_SIZE];     /*The type of the resource.*/
    char key[CGRW_KEY_BUFF_SIZE];      /*The key of the resource.*/
    char path[CGRW_PATH_BUFF_SIZE];     /*The path of the resource.*/
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
char* CGRWLoadResource(const char* resource_key);

CGRWResourceData* CGRWPhraseUsedResource(const char* file_path);

/**
 * @brief Terminate CG resource wrapper.
 */
void CGRWTerminate();

#ifdef __cplusplus
}
#endif
#endif  // _CGRW_RESOURCE_WRAPPER_H_