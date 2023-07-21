#ifndef _CG_LOG_H_
#define _CG_LOG_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CG_MODIFY_OUTPUT_MESSAGE
    #define CG_PRINT(...) CGPrint("CosGraphics", "Output", __VA_ARGS__)
    #define CG_PRINT_WITH_FUNCTION(...) CGPrintWithFunction("CosGraphics", "Output", __FILE__, __FUNCTION__, __VA_ARGS__)
    #define CG_ERROR(...) CGPrintWithFunction("CosGraphics", "Error", __FILE__, __FUNCTION__, __VA_ARGS__)
    #define CG_WARNING(...) CGPrintWithFunction("CosGraphics", "Warning", __FILE__, __FUNCTION__, __VA_ARGS__)
#endif

/**
 * @brief Print out message. The message will follow format: [sender] <type>: message
 * 
 * @param sender The sender of the message.
 * @param type The type of the message.
 * @param fmt The format of the message.
 * @param ... The arguments of the message.
 */
void CGPrint(const char* sender, const char* type, const char* fmt, ...);

/**
 * @brief Print out message with file and function information. The message will follow format: 
 * [sender] <type>: file: file_name, function function_name: message
 * 
 * @param sender The sender of the message.
 * @param type The type of the message.
 * @param file_name The name of the file.
 * @param function_name The name of the function.
 * @param fmt The format of the message.
 * @param ... 
 */
void CGPrintWithFunction(const char* sender, const char* type, const char* file_name, const char* function_name, const char* fmt, ...);

/**
 * @brief Should print out error. If condition is true, then print out error and exit the function.
 */
#define CG_ERROR_CONDITION(cond, ...) \
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
