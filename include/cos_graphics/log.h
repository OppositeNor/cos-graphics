#ifndef _CG_LOG_H_
#define _CG_LOG_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CG_MODIFY_OUTPUT_MESSAGE
    #define CG_PRINT(...) CGPrint("CosGraphics", "Output", __VA_ARGS__)
    #define CG_ERROR(...) CGPrintWithFunction("CosGraphics", "Error", __FUNCTION__, __VA_ARGS__)
    #define CG_WARNING(...) CGPrint("CosGraphics", "Warning", __FUNCTION__, __VA_ARGS__)
#endif

void CGPrint(const char* sender, const char* type, const char* fmt, ...);

#include <stdlib.h>
void CGPrintWithFunction(const char* sender, const char* type, const char* function_name, const char* fmt, ...);

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
