#ifndef _CG_LOG_H_
#define _CG_LOG_H_

#include <stdlib.h>
#include "cos_graphics/defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CG_MODIFY_OUTPUT_MESSAGE

#ifndef CG_NO_ERROR_NOTIFICATION
    #define CG_PRINT(...) CGPrint(CGSTR("CosGraphics"), CGSTR("Output"), __VA_ARGS__)
    #define CG_PRINT_WITH_FUNCTION(...) CGPrintWithFunction(CGSTR("CosGraphics"), CGSTR("Output"), __FUNCTION__, __VA_ARGS__)
    #define CG_ERROR(...) CGError(); CGPrintWithFunction(CGSTR("CosGraphics"), CGSTR("Error"), __FUNCTION__, __VA_ARGS__)
    #define CG_WARNING(...) CGPrintWithFunction(CGSTR("CosGraphics"), CGSTR("Warning"), __FUNCTION__, __VA_ARGS__)
#else
    #define CG_PRINT(...)
    #define CG_PRINT_WITH_FUNCTION(...)
    #define CG_ERROR(...) CGError();
    #define CG_WARNING(...)
#endif

#endif


#ifdef CG_PRINT_VERBOSE
    #define CG_PRINT_VERBOSE(...) CG_PRINT(__VA_ARGS__)
    #define CG_PRINT_VERBOSE_WITH_FUNCTION(...) CG_PRINT_WITH_FUNCTION(__VA_ARGS__)
#else
    #define CG_PRINT_VERBOSE(...)
    #define CG_PRINT_VERBOSE_WITH_FUNCTION(...)
#endif

/**
 * @brief Called when an error occurs.
 */
void CGError();

/**
 * @brief Returns whether an error has occurred.
 * @return CG_TRUE if an error has occurred, CG_FALSE otherwise.
 */
CG_BOOL CGIsHasError();

/**
 * @brief Reset the error flag.
 * 
 */
void CGResetError();

/**
 * @brief Print out message. The message will follow format: [sender] <type>: message
 * 
 * @param sender The sender of the message.
 * @param type The type of the message.
 * @param fmt The format of the message.
 * @param ... The arguments of the message.
 */
void CGPrint(const CGChar* sender, const CGChar* type, const CGChar* fmt, ...);

/**
 * @brief Print out message with file and function information. The message will follow format: 
 * [sender] <type>: file: file_name, function function_name: message
 * 
 * @param sender The sender of the message.
 * @param type The type of the message.
 * @param function_name The name of the function.
 * @param fmt The format of the message.
 * @param ... 
 */
void CGPrintWithFunction(const CGChar* sender, const CGChar* type, const char* function_name, const CGChar* fmt, ...);

/**
 * @brief Should print out error. If condition is true, then print out error and exit the function.
 */
#define CG_ERROR_CONDITION(cond, ...)   \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return;                     \
        }((void)0)
/**
 * @brief Should print out error. If condition is true, then print out error and return the return_value.
 */
#define CG_ERROR_COND_RETURN(cond, return_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return return_value;        \
        }                               \
        ((void)0)
/**
 * @brief Should print out error. If condition is true, then print out error and crash the program.
 */
#define CG_ERROR_COND_EXIT(cond, exit_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            exit(exit_value);           \
        }((void)0)


#ifdef __cplusplus
}
#endif

#endif  // _CG_LOG_H_
